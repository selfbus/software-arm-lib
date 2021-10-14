/*
 *  update.cpp  - UPD/UDP Selfbus protocol implementation.
 *
 *  Copyright (c) 2015 Martin Glueck <martin@mangari.org>
 *  Copyright (c) 2021 Stefan Haller
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib.h>
#include <sblib/eib/bus.h>
#include <sblib/eib/apci.h>
#include <sblib/timeout.h>
#include <sblib/internal/iap.h>
#include <string.h>
#include <sblib/io_pin_names.h>
#include "bcu_updater.h"
#include "crc.h"
#include "boot_descriptor_block.h"
#include "update.h"

#ifdef DECOMPRESSOR
#   include <Decompressor.h>
#endif

#ifdef DUMP_TELEGRAMS_LVL1
#   include <sblib/serial.h>
#   define d1(x) {serial.print(x);}
#   define dline(x) {serial.println(x);}
#   define d2(u,v,w) {serial.print(u,v,w);}
#else
#   define d1(x)
#   define d2(u,v,w)
#endif

#ifdef DECOMPRESSOR
    Decompressor decompressor((AppDescriptionBlock*) BOOT_DSCR_ADDRESS); // get application base address from boot descriptor
#endif


#define DEVICE_LOCKED   ((unsigned int ) 0x5AA55AA5)    //!< \todo magic number for device is locked
                                                        //!< and can't be flashed?
#define DEVICE_UNLOCKED ((unsigned int ) ~DEVICE_LOCKED) //!< \todo magic number for device is unlocked
                                                         //!< and flashing is allowed?
#define ADDRESS2SECTOR(a) ((a + RAM_BUFFER_SIZE - 1) / RAM_BUFFER_SIZE) //!> address to sector conversion

#define PAGE_ALIGNMENT 0xff             //!< page alignement which is allowed to flash

unsigned char ramBuffer[FLASH_SECTOR_SIZE]; //!< RAM buffer used for flash operations

/**
 * @brief UDP_Command \todo
 *
 */
enum UDP_Command
{
    UPD_ERASE_SECTOR = 0,               //!< UPD_ERASE_SECTOR
    UPD_SEND_DATA = 1,                  //!< UPD_SEND_DATA
    UPD_PROGRAM = 2,                    //!< UPD_PROGRAM
    UPD_UPDATE_BOOT_DESC = 3,           //!< UPD_UPDATE_BOOT_DESC
    UPD_SEND_DATA_TO_DECOMPRESS = 4,    //!< Test
    UPD_PROGRAM_DECOMPRESSED_DATA = 5,  //!< Test

    UPD_REQ_DATA = 10,                  //!< UPD_REQ_DATA

    UPD_GET_LAST_ERROR = 20,            //!< UPD_GET_LAST_ERROR
    UPD_SEND_LAST_ERROR = 21,           //!< UPD_SEND_LAST_ERROR

    UPD_UNLOCK_DEVICE = 30,             //!< UPD_UNLOCK_DEVICE
    UPD_REQUEST_UID = 31,               //!< UPD_REQUEST_UID
    UPD_RESPONSE_UID = 32,              //!< UPD_RESPONSE_UID
    UPD_APP_VERSION_REQUEST = 33,       //!< UPD_APP_VERSION_REQUEST
    UPD_APP_VERSION_RESPONSE = 34,      //!< UPD_APP_VERSION_RESPONSE
    UPD_RESET = 35,                     //!< UPD_RESET
    UPD_REQUEST_BOOT_DESC = 36,         //!< UPD_REQUEST_BOOT_DESC
    UPD_RESPONSE_BOOT_DESC = 37,        //!< UPD_RESPONSE_BOOT_DESC

    UPD_REQUEST_BL_IDENTITY = 40,       //!< UPD_REQUEST_BL_IDENTITY
    UPD_RESPONSE_BL_IDENTITY = 41,      //!< UPD_RESPONSE_BL_IDENTITY
};


/**
 * @brief UPD_Status \todo
 *
 */
enum UPD_Status
{
    UDP_UNKONW_COMMAND = 0x100,       //!< received command is not defined
    UDP_CRC_ERROR,                    //!< CRC calculated on the device and by the updater don't match
    UPD_ADDRESS_NOT_ALLOWED_TO_FLASH, //!< specifed address cannot be programmed
    UPD_SECTOR_NOT_ALLOWED_TO_ERASE,  //!< the specified sector cannot be erased
    UPD_RAM_BUFFER_OVERFLOW,          //!< internal buffer for storing the data would overflow
    UPD_WRONG_DESCRIPTOR_BLOCK,       //!< the boot descriptor block does not exist
    UPD_APPLICATION_NOT_STARTABLE,    //!< the programmed application is not startable
    UPD_DEVICE_LOCKED,                //!< the device is still locked
    UPD_UID_MISMATCH,                 //!< UID sent to unlock the device is invalid
    UPD_ERASE_FAILED,                 //!< page erase failed
    UPD_FLASH_ERROR,                  //!< page program (flash) failed
    UDP_NOT_IMPLEMENTED = 0xFFFF      //!< this command is not yet implemented
};

/**
 * @brief The vector table marks the beginning of the updater application
 *        used to protect the updater from killing itself with
 *        a new application downloaded over the bus
 */
extern const unsigned int __vectors_start__;

/**
 * @brief _etext symbol marks the end of the used flash area
 *        used to protect the updater from killing itself with
 *        a new application downloaded over the bus
 */
extern const unsigned int _etext;

Timeout mcuRestartRequestTimeout; //!< Timeout used to trigger a MCU Reset by NVIC_SystemReset()

void restartRequest (unsigned int msec)
{
#ifdef DUMP_TELEGRAMS_LVL1
    serial.print("Systime:", systemTime, DEC);
    serial.print(" restartReq");
#endif
	mcuRestartRequestTimeout.start(msec);
}

bool restartRequestExpired(void)
{
	return mcuRestartRequestTimeout.expired();
}

/**
 * @brief Converts a 4 byte long provided buffer into a unsigned int
 * @detail A direct cast does not work due to possible miss aligned addresses.
 *         therefore a good old conversion has to be performed
 *
 * @param buffer data to convert
 * @return to unsigned int converted value of the 4 first bytes of buffer
 * @warning function doesn't perform any sanity-checks on the provided buffer
 */
unsigned int streamToUIn32(unsigned char * buffer)
{
    return buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0];
}

/**
 * @brief Converts a unsigned int into a 4 byte long provided buffer
 * @detail A direct cast does not work due to possible miss aligned addresses.
 *         therefore a good old conversion has to be performed
 *
 * @param buffer in the 4 first bytes of buffer the result will be stored
 * @warning function doesn't perform any sanity-checks on the provided buffer
 */
void UIn32ToStream(unsigned char * buffer, unsigned int val)
{
    buffer[3] = val >> 24;
    buffer[2] = val >> 16;
    buffer[1] = val >> 8;
    buffer[0] = val & 0xff;
}

static bool _prepareReturnTelegram(unsigned int count, unsigned char cmd)
{
    bcu.sendTelegram[5] = 0x63 + count;
    bcu.sendTelegram[6] = 0x42;
    bcu.sendTelegram[7] = 0x40 | count;
    bcu.sendTelegram[8] = 0;
    bcu.sendTelegram[9] = cmd;
    return true;
}

//// DEBUG delete and use code in other file if done here
//unsigned int checkVectorTableDBG(unsigned int start)
//{
//    unsigned int i;
//    unsigned int * address;
//    unsigned int cs = 0;
//    address = (unsigned int *) start;	// Vector table start always at base address, each entry is 4 byte
//
//    for (i = 0; i < 7; i++)				// Checksum is 2's complement of entries 0 through 6
//        cs += address[i];
//    //if (address[7])
//
//    d1("\n\rVT Check 0 ");
//    d2(address[0],HEX,8);
//    d1("VT Check 1 ");
//	d2(address[1],HEX,8);
//	d1("VT Check 2 ");
//	d2(address[2],HEX,8);
//	d1("VT Check 3 ");
//	d2(address[3],HEX,8);
//	d1("VT Check 4 ");
//	d2(address[4],HEX,8);
//	d1("VT Check 5 ");
//	d2(address[5],HEX,8);
//	d1("VT Check 6 ");
//	d2(address[6],HEX,8);
//	d1("VT Check Res ");
//	d2(address[7],HEX,8);
//	d1("Should be == ");
//	d2(~cs+1,HEX,8);
//
//    return (~cs+1);
//}


/*
 * Checks if the requested sector is allowed to be erased.
 */
inline bool sectorAllowedToErease(unsigned int sectorNumber)
{
    if (sectorNumber < ADDRESS2SECTOR(FIRST_SECTOR-1)) // last byte of bootloader
        return false; // protect bootloader sectors
    return !((sectorNumber >= ADDRESS2SECTOR(__vectors_start__))
            && (sectorNumber <= ADDRESS2SECTOR(_etext)));
}

/**
 * @brief Checks if the address range is allowed to be programmed
 *
 * @param start Start of the address range to check
 * @param length length of the address range
 * @return true if programming is allowed, otherwise false
 */
inline bool addressAllowedToProgram(unsigned int start, unsigned int length)
{
    unsigned int end = start + length;
    if ((start & 0xff) || !length) // not aligned to page or 0 length
    {
        return 0;
    }
    return !((start >= __vectors_start__) && (end <= _etext));
}

unsigned int request_flashWrite(unsigned char* data, bool write_request)
{
	static volatile bool ready2write = false;
	unsigned int crc;
	static volatile unsigned int flash_count;
	static volatile unsigned int address;

	if (write_request)
	{
		flash_count = streamToUIn32(data + 3);
		address = streamToUIn32(data + 3 + 4);
		if (addressAllowedToProgram(address, flash_count))
		{
			crc = crc32(0xFFFFFFFF, ramBuffer, flash_count);
			if (crc == streamToUIn32(data + 3 + 4 + 4))
			{
				// Select smallest possible sector size
				if (flash_count > 4*FLASH_PAGE_SIZE)
					flash_count = 8*FLASH_PAGE_SIZE;
				else if (flash_count > 2*FLASH_PAGE_SIZE)
					flash_count = 4*FLASH_PAGE_SIZE;
				else if (flash_count > FLASH_PAGE_SIZE)
					flash_count = 2*FLASH_PAGE_SIZE;
				else
					flash_count = FLASH_PAGE_SIZE;

				// Debug: write CRC
				d2(crc,HEX,8);
				d1(" Prepare... ");

				// Everything is setup, ready to write to flash
				ready2write = true;

				return IAP_SUCCESS;	//UDP_FLASH_WRITE_SETUP;
			}
			else
				return UDP_CRC_ERROR;
		}
		else
			return UPD_ADDRESS_NOT_ALLOWED_TO_FLASH;
	}
	else if (ready2write)
	{
		ready2write = false;
		d1(" flash!\n\r");
		return iapProgram((byte *) address, ramBuffer,flash_count);
	}
	return 0;
}

unsigned char handleMemoryRequests(int apciCmd, bool * sendTel,
        unsigned char * data)
{
    unsigned int count = data[0] & 0x0f;
    unsigned int address;
    unsigned int bl_identity = BL_IDENTITY;
    unsigned int bl_features = BL_FEATURES;
    static unsigned int ramLocation;
    static unsigned int deviceLocked = DEVICE_LOCKED;
    unsigned int crc = 0xFFFFFFFF;
    static unsigned int lastError = 0;
    unsigned int sendLastError = 0;

    static unsigned char bl_id_string[13] = BL_ID_STRING;
    digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));
    switch (data[2])
    {
        case UPD_UNLOCK_DEVICE:
        	d1("Unlock? ");
            if (((BcuUpdate &) bcu).progPinStatus())
            { // the operator has physical access to the device -> we unlock it
                deviceLocked = DEVICE_UNLOCKED;
                lastError = IAP_SUCCESS;
                d1("Btn\n\r");
            }
            else
            {   // we need to ensure that only authorized operators can
                // update the application
                // as a simple method we use the unique ID of the CPU itself
                // only if this UUID is known, the device will be unlocked
                byte uid[4 * 32];
                lastError = IAP_SUCCESS;
                if (IAP_SUCCESS == iapReadUID(uid))
                {
                    for (unsigned int i = 0; i < 12; i++)
                    {
                        if (data[i + 3] != uid[i])
                        {
                            lastError = UPD_UID_MISMATCH;
                        }
                    }
                }
                if (lastError != UPD_UID_MISMATCH)
                {
                    deviceLocked = DEVICE_UNLOCKED;
                    lastError = IAP_SUCCESS;
                    d1("UID\n\r");
                }
            }
            sendLastError = true;
            ramLocation = 0;
            //crc = 0xFFFFFFFF; // Todo, check if this can be removed, it's initialized always to same value
            break;
        case UPD_REQUEST_UID:
        	d1("UID? ");
            if (((BcuUpdate &) bcu).progPinStatus())
            { // the operator has physical access to the device -> we unlock it
                byte uid[4 * 4];
                lastError = iapReadUID(uid);
                if (lastError == IAP_SUCCESS)
                {
                    *sendTel = _prepareReturnTelegram(12, UPD_RESPONSE_UID);
                    memcpy(bcu.sendTelegram + 10, uid, 12);
                }
                d1("OK\n\r");
                break;
            }
            else
            {
                lastError = UPD_DEVICE_LOCKED;
            	d1("LOCK\n\r");
            	sendLastError = true;
            	break;
            }
        case UPD_RESET:
        	d1("Restart!");
            if (deviceLocked == DEVICE_UNLOCKED)
            {
            	restartRequest(RESET_DELAY_MS);	// request restart in RESET_DELAY_MS ms to allow transmission of ACK before
            	lastError = IAP_SUCCESS;
            }
            else
            {
                lastError = UPD_DEVICE_LOCKED;
            }
        	sendLastError = true;
            break;
        case UPD_APP_VERSION_REQUEST:
        	d1("App_Version Request ");
            unsigned char * appversion;
            appversion = getAppVersion(
                    (AppDescriptionBlock *) (FIRST_SECTOR - (1 + data[3]) * BOOT_BLOCK_DESC_SIZE));
            if (((unsigned int) appversion) > 0xFFFF) // Limit to our 64k flash
            {
                d1("Outside range!\n\r");
                appversion = bl_id_string;			// Bootloader ID if invalid (address out of range)
            }
            else
            {
                d1("OK\n\r");
            }
            *sendTel = _prepareReturnTelegram(12, UPD_APP_VERSION_RESPONSE);
            memcpy(bcu.sendTelegram + 10, appversion, 12);
            break;
        case UPD_ERASE_SECTOR:
        	d1("Erase Sector ");
            if (deviceLocked == DEVICE_UNLOCKED)
            {
            	d2(data[3],DEC,1);
                if (sectorAllowedToErease(data[3]))
                {
                    lastError = iapEraseSector(data[3]);
                    d1(" OK\n\r");
                }
                else
                {
                    lastError = UPD_SECTOR_NOT_ALLOWED_TO_ERASE;
                    d1(" Failed!\n\r");
                }
            }
            else
                lastError = UPD_DEVICE_LOCKED;
            ramLocation = 0;
            sendLastError = true;
            break;
        case UPD_SEND_DATA:
        	d1("Receive Data: ");
            if (deviceLocked == DEVICE_UNLOCKED)
            {
            	ramLocation = data[3];// * 11;	// Current Byte position as message number with 11 Bytes payload each
            	count --; 					// 1 Bytes abziehen, da diese für die Nachrichtennummer verwendet wird
            	if ((ramLocation + count) <= sizeof(ramBuffer))	// Space left
                {
            		memcpy((void *) &ramBuffer[ramLocation], data + 4, count); //ab dem 4. Byte sind die Daten verfügbar
                    //crc = crc32(crc, data + 3, count);
                    lastError = IAP_SUCCESS;
                    //d1("OK\n\r");
                    for(unsigned int i=0; i<count; i++){
                    	d2(data[i+4],HEX,2);
                    	d1(" ");
                    }
                    d1("at data location: ");
                    d2(ramLocation,DEC,3);
                    d1("\r\n");
                }
                else
                {
                    lastError = UPD_RAM_BUFFER_OVERFLOW;
                	d1("Buffer Full\n\r");
                }
            }
            else
            {
                lastError = UPD_DEVICE_LOCKED;
            }
            sendLastError = true;
            break;
        case UPD_PROGRAM:
        	d1("\n\rFlash Sector, CRC 0x");
            if (deviceLocked == DEVICE_UNLOCKED)
            	lastError = request_flashWrite(data, 1);
            else
                lastError = UPD_DEVICE_LOCKED;
            //crc = 0xFFFFFFFF;	//Todo, remove this line
            sendLastError = true;
            break;

#ifdef DECOMPRESSOR
		case UPD_SEND_DATA_TO_DECOMPRESS:
			d1("#");
			if (deviceLocked == DEVICE_UNLOCKED)
			{
				if ((ramLocation + count) <= sizeof(ramBuffer)) // Check if this can be removed. Overflow protection should be in decompressor class instead!
				{
					for (unsigned int i = 0; i < count; i++) {
						decompressor.putByte(data[i + 3]);
					};
					lastError = IAP_SUCCESS;
				}
				else
					lastError = UPD_RAM_BUFFER_OVERFLOW;
			}
			else
				lastError = UPD_DEVICE_LOCKED;
			sendLastError = true;
			break;

        case UPD_PROGRAM_DECOMPRESSED_DATA:
             if (deviceLocked == DEVICE_UNLOCKED)
             {
                count = decompressor.getBytesCountToBeFlashed();
                address = decompressor.getStartAddrOfPageToBeFlashed();

                d1("\n\rFlash Diff address 0x");
                d2(address,HEX,4);
                d1(" length: ");
                d2(count,DEC,3);

                if (addressAllowedToProgram(address, count))
                {
                	d1(" Address valid, ");
                	crc = decompressor.getCrc32();
                	if (crc == streamToUIn32(data + 3 + 4 + 4))
					{
                		d1("CRC OK,\n\r");
                	    if (decompressor.pageCompletedDoFlash())
                	    	lastError = UPD_FLASH_ERROR;
                	    else
                	    	lastError = IAP_SUCCESS;
					}
                	else
                	{
                		d1("CRC Error!\n\r");
                		lastError = UDP_CRC_ERROR;
                	}
                 }
                 else
                 {
                	 d1(" Address protected!\n\r");
                     lastError = UPD_ADDRESS_NOT_ALLOWED_TO_FLASH;
                 }
             }
             else
                 lastError = UPD_DEVICE_LOCKED;
             ramLocation = 0;
             //crc = 0xFFFFFFFF;
             sendLastError = true;
             break;
#endif

        case UPD_UPDATE_BOOT_DESC:
        	d1("\n\rBOOT_Desc ");
            if ((deviceLocked == DEVICE_UNLOCKED))// && (data[7] < 2)) // Test boot descriptor block number for valid value
            {
            	d1("Unlocked, ");
            	count = streamToUIn32(data + 3);			// length of the descriptor
                crc = crc32(0xFFFFFFFF, ramBuffer, count);	// checksum on used length only
                //address = FIRST_SECTOR - (1 + data[7]) * BOOT_BLOCK_SIZE; // start address of the descriptor block
                address = BOOT_DSCR_ADDRESS;				// Address of boot block descriptor

                d1("Desc. CRC 0x");
                d2(crc,HEX,8);

                d1("  Desc. at address 0x");
                d2(address,HEX,4);

                d1("\n\rFW start@ 0x");
				d2(streamToUIn32(ramBuffer),HEX,4);		// FW start address
				d1("\n\rFW end  @ 0x");
				d2(streamToUIn32(ramBuffer+4),HEX,4);	// FW end address
				d1("\n\rFWs CRC : 0x");
				d2(streamToUIn32(ramBuffer+8),HEX,8);	// FW CRC
				d1("\n\rFW Desc.@ 0x");
				d2(streamToUIn32(ramBuffer+12),HEX,4);	// FW App descriptor address
				//d1("\n\rRamBuffer[16-20] ");
				//d2(streamToUIn32(ramBuffer+16),HEX,8);// This is beyond the descriptor block

				d1("\n\rReceived CRC 0x");
				d2(streamToUIn32(data + 7),HEX,8);

				if (crc == streamToUIn32(data + 7))
                {
					d1("\n\rCRC MATCH, comparing MCUs BootDescriptor: ");
					if(memcmp((byte *) address, ramBuffer, count)) //If send descriptor is not equal to current one, flash it
					{
						d1("it's different, Erase Page: ");

						if (checkApplication((AppDescriptionBlock *) ramBuffer))
						{
							lastError = iapErasePage(BOOT_BLOCK_PAGE);// - data[7]);
							d2(lastError,DEC,2);
							if (lastError == IAP_SUCCESS)
							{
								d1(" OK, Flash Page: ");
								lastError = iapProgram((byte *) address, ramBuffer, FLASH_PAGE_SIZE); // no less than 256byte can be flashed
								d2(lastError,DEC,2);
							}
						}
						else
						{
							lastError = UPD_APPLICATION_NOT_STARTABLE;
							d1("App Error\n\r");
						}
					}
					else
					{
						d1("is equal, skipping\n\r");
						lastError = IAP_SUCCESS;
					}
                }
                else
                {
                    lastError = UDP_CRC_ERROR;

                    d1(" data[3-0]:");
                    d2(streamToUIn32(data),HEX,8);
                    d1(" data[7-4]:");
                    d2(streamToUIn32(data+4),HEX,8);
                    d1(" data[11-8]:");
                    d2(streamToUIn32(data+8),HEX,8);
                    d1(" data[15-12]:");
                    d2(streamToUIn32(data+12),HEX,8);
                }
            }
            else
            {
                lastError = UPD_DEVICE_LOCKED;
                d1("Lock\n\r");
            }
            //crc = 0xFFFFFFFF;
            sendLastError = true;
            break;

		case UPD_REQUEST_BOOT_DESC:
			d1("BOOT_Desc ?\n\r");
			if (deviceLocked == DEVICE_UNLOCKED)
			{
				AppDescriptionBlock* bootDescr = (AppDescriptionBlock *) BOOT_DSCR_ADDRESS;	// Address of boot block descriptor
				if (lastError == IAP_SUCCESS)
				{
					*sendTel = _prepareReturnTelegram(12, UPD_RESPONSE_BOOT_DESC);
					memcpy(bcu.sendTelegram + 10, bootDescr, 12); // startAddress, endAddress, crc
				}
				break;
			}
			else
			{
				lastError = UPD_DEVICE_LOCKED;
				sendLastError = true;
				break;
			}

		case UPD_REQUEST_BL_IDENTITY:
			d1("BL_Identity ?\n\r");
			if (deviceLocked == DEVICE_UNLOCKED)
			{
				if (lastError == IAP_SUCCESS)
				{
					*sendTel = _prepareReturnTelegram(12, UPD_RESPONSE_BL_IDENTITY);
					UIn32ToStream(bcu.sendTelegram + 10, bl_identity);	// Bootloader identity
					UIn32ToStream(bcu.sendTelegram + 14, bl_features);	// Bootloader features
					UIn32ToStream(bcu.sendTelegram + 18, FIRST_SECTOR);	// Bootloader size
				}
				break;
			}
			else
			{
				lastError = UPD_DEVICE_LOCKED;
				sendLastError = true;
				break;
			}

        case UPD_REQ_DATA:
            if (deviceLocked == DEVICE_UNLOCKED)
            {
                /*
                 memcpy(bcu.sendTelegram + 9, (void *)address, count);
                 bcu.sendTelegram[5] = 0x63 + count;
                 bcu.sendTelegram[6] = 0x42;
                 bcu.sendTelegram[7] = 0x40 | count;
                 bcu.sendTelegram[8] = UPD_SEND_DATA;
                 * sendTel = true;
                 * */
                lastError = UDP_NOT_IMPLEMENTED; // set to any error
            }
            else
                lastError = UPD_DEVICE_LOCKED;
            sendLastError = true;
            break;
        case UPD_GET_LAST_ERROR:
        	sendLastError = true;
            break;
        default:
            lastError = UDP_UNKONW_COMMAND; // set to any error
        	sendLastError = true;
    }
    if (sendLastError)
    {
        *sendTel = _prepareReturnTelegram(4, UPD_SEND_LAST_ERROR);
        UIn32ToStream(bcu.sendTelegram + 10, lastError);
    }
    return T_ACK_PDU;
}
