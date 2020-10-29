/*
 *  app_main.cpp - The application's main.
 *
 *  Copyright (c) 2015 Martin Glueck <martin@mangari.org>
 *  Copyright (c) 2020 Stefan Haller
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
#include <sblib/serial.h>
#include <bcu_updater.h>
#include <crc.h>
#include <boot_descriptor_block.h>


#ifdef DECOMPRESSOR
#include <Decompressor.h>
#endif

/**
 * Updater protocol:
 *   We miss-use the memory write EIB frames. Miss-use because we do not transmit the address in each request
 *   to have more frame left for the actual data transmission:
 *     BYTES of the EIB telegram:
 *       8    CMD Number (see enum below), this is data[2]
 *       9-x  CMD dependent
 *
 *    UPD_ERASE_SECTOR
 *      9    Number of the sector which should be erased
 *           if the erasing was successful a T_ACK_PDU will be returned, otherwise a T_NACK_PDU
 *    UPD_SEND_DATA
 *      9-   the actual data which will be copied into a RAM buffer for later use
 *           If the RAM buffer is not yet full a T_ACK_PDU will be returned, otherwise a T_NACK_PDU
 *           The address of the RAM buffer will be automatically incremented.
 *           After a Program or Boot Desc. update, the RAM buffer address will be reseted.
 *    UPD_PROGRAM
 *      9-12 How many bytes of the RMA Buffer should be programmed. Be aware that the value needs to be one of the following
 *           256, 512, 1024, 4096 (required by the IAP of the LPC11xx devices)
 *     13-16 Flash address the data should be programmed to
 *     16-19 The CRC of the data downloaded via the UPD_SEND_DATA commands. If the CRC does not match the
 *           programming, error is returned
 *    UPD_UPDATE_BOOT_DESC
 *    UPD_PROGRAM
 *      9-12 The CRC of the data downloaded via the UPD_SEND_DATA commands. If the CRC does not match the
 *           programming, error is returned
 *        13 Which boot block should be used
 *    UPD_REQ_DATA
 *      ???
 *    UPD_GET_LAST_ERROR
 *      Returns the reason why the last memory write PDU had a T_NACK_PDU
 *
 *    Workflow:
 *      - erase the sector which needs to be programmed (UPD_ERASE_SECTOR)
 *      - download the data via UPD_SEND_DATA telegrams
 *      - program the transmitted data to into the FLASH  (UPD_PROGRAM)
 *      - repeat the above steps until the whole application has been downloaded
 *      - download the boot descriptor block via UPD_SEND_DATA telegrams
 *      - update the boot descriptor block so that the bootloader is able to start the new
 *        application (UPD_UPDATE_BOOT_DESC)
 *      - restart the board (UPD_RESTART)
 */


#ifdef DUMP_TELEGRAMS
#define d1(x) {serial.print(x);}
#define d2(u,v,w) {serial.print(u,v,w);}
#else
#define d1(x)
#define d2(u,v,w)
#endif


enum
{
    UPD_ERASE_SECTOR = 0,
    UPD_SEND_DATA = 1,
    UPD_PROGRAM = 2,
    UPD_UPDATE_BOOT_DESC = 3,
	UPD_SEND_DATA_TO_DECOMPRESS = 4,	// Test
	UPD_PROGRAM_DECOMPRESSED_DATA = 5,	// Test
    UPD_REQ_DATA = 10,
    UPD_GET_LAST_ERROR = 20,
    UPD_SEND_LAST_ERROR = 21,
    UPD_UNLOCK_DEVICE = 30,
    UPD_REQUEST_UID = 31,
    UPD_RESPONSE_UID = 32,
    UPD_APP_VERSION_REQUEST = 33,
    UPD_APP_VERSION_RESPONSE = 34,
    UPD_RESET = 35,
	UPD_REQUEST_BOOT_DESC = 36,
	UPD_RESPONSE_BOOT_DESC = 37,
	UPD_REQUEST_BL_IDENTITY = 40,
	UPD_RESPONSE_BL_IDENTITY = 41,
};

#define DEVICE_LOCKED   ((unsigned int ) 0x5AA55AA5)
#define DEVICE_UNLOCKED ((unsigned int ) ~DEVICE_LOCKED)
#define ADDRESS2SECTOR(a) ((a + 4095) / 4096)

enum UPD_Status
{
    UDP_UNKONW_COMMAND = 0x100       //<! received command is not defined
    ,
    UDP_CRC_ERROR                     //<! CRC calculated on the device
                                      //<! and by the updater don't match
    ,
    UPD_ADDRESS_NOT_ALLOWED_TO_FLASH //<! specifed address cannot be programmed
    ,
    UPD_SECTOR_NOT_ALLOWED_TO_ERASE  //<! the specified sector cannot be erased
    ,
    UPD_RAM_BUFFER_OVERFLOW          //<! internal buffer for storing the data
                                     //<! would overflow
    ,
    UPD_WRONG_DESCRIPTOR_BLOCK     //<! the boot descriptor block does not exist
    ,
    UPD_APPLICATION_NOT_STARTABLE //<! the programmed application is not startable
    ,
    UPD_DEVICE_LOCKED                //<! the device is still locked
    ,
    UPD_UID_MISMATCH               //<! UID sent to unlock the device is invalid
	,
	UPD_ERASE_FAILED				// page erase failed
	,
	UPD_FLASH_ERROR					// page program (flash) failed
    ,
    UDP_NOT_IMPLEMENTED = 0xFFFF    //<! this command is not yet implemented
};

unsigned char ramBuffer[4096];

#ifdef DECOMPRESSOR
Decompressor decompressor((AppDescriptionBlock*) BOOT_DSCR_ADDRESS);	// get application base address from boot descriptor
#endif

Timeout mcu_restart_request;

void restartRequest (unsigned int time)
{
	mcu_restart_request.start(time);
}

bool restartRequestExpired(void)
{
	return mcu_restart_request.expired();
}



/*
 * a direct cast does not work due to possible miss aligned addresses.
 * therefore a good old conversion has to be performed
 */
unsigned int streamToUIn32(unsigned char * buffer)
{
    return buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0];
}

void UIn32ToStream(unsigned char * buffer, unsigned int val)
{
    buffer[3] = val >> 24;
    buffer[2] = val >> 16;
    buffer[1] = val >> 8;
    buffer[0] = val & 0xff;
}

/* the following two symbols are used to protect the updater from
 * killing itself with a new application downloaded over the bus
 */
/* the vector table marks the beginning of the updater application */
extern const unsigned int __vectors_start__;
/* the _etext symbol marks the end of the used flash area */
extern const unsigned int _etext;

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

/*
 * Checks if the address range is allowed to be programmed
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
            crc = 0xFFFFFFFF;
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
            }
            break;
        case UPD_RESET:
        	d1("Restart!");
            if (deviceLocked == DEVICE_UNLOCKED)
            {
            	restartRequest(100);	// request restart in 100ms to allow transmission of ACK before
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
        	d1("\r\033[2KReceive Data: ");
            if (deviceLocked == DEVICE_UNLOCKED)
            {
                if ((ramLocation + count) <= sizeof(ramBuffer))
                {
                    memcpy((void *) &ramBuffer[ramLocation], data + 3, count);
                    //crc = crc32(crc, data + 3, count);
                    ramLocation += count;
                    lastError = IAP_SUCCESS;
                    //d1("OK\n\r");
                    d2(ramLocation,DEC,4);
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
            {
                count = streamToUIn32(data + 3);
                address = streamToUIn32(data + 3 + 4);
                if (addressAllowedToProgram(address, count))
                {
                    crc = crc32(0xFFFFFFFF, ramBuffer, count);
                    if (crc == streamToUIn32(data + 3 + 4 + 4))
                    {
                        if (count > 1024)
                        {
                            count = 4096;
                        }
                        else if (count > 512)
                        {
                            count = 1024;
                        }
                        else if (count > 256)
                        {
                            count = 512;
                        }
                        else
                        {
                            count = 256;
                        }
                        lastError = iapProgram((byte *) address, ramBuffer,count);

                        d2(crc,HEX,8);
                        d1("\n\r");
                    }
                    else
                        lastError = UDP_CRC_ERROR;
                }
                else
                    lastError = UPD_ADDRESS_NOT_ALLOWED_TO_FLASH;
            }
            else
                lastError = UPD_DEVICE_LOCKED;
            ramLocation = 0;
            crc = 0xFFFFFFFF;
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
             crc = 0xFFFFFFFF;
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
								lastError = iapProgram((byte *) address, ramBuffer, 256); // no less than 256byte can be flashed
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
            ramLocation = 0;
            crc = 0xFFFFFFFF;
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
				lastError = UPD_DEVICE_LOCKED;
			sendLastError = true;
			break;

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
				lastError = UPD_DEVICE_LOCKED;
			sendLastError = true;
			break;

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
