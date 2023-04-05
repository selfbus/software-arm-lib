/*
 *  bcu2.h - BCU 2 specific stuff.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_bcu2_h
#define sblib_bcu2_h

#include <sblib/eib/bcu1.h>
#include <sblib/eib/userEepromBCU2.h>
#include <sblib/eib/userRamBCU2.h>
#include <sblib/eib/propertiesBCU2.h>
#include <sblib/eib/com_objectsBCU2.h>
#include <sblib/eib/addr_tablesBCU2.h>

/**
 * Class for controlling all BCU related things.
 *
 * In order to use the EIB bus, you need to call bcu.begin() once in your application's
 * setup() function.
 */
class BCU2 : public BcuDefault
{
public:
	BCU2();
	BCU2(UserRamBCU2* userRam, UserEepromBCU2* userEeprom, ComObjectsBCU2* comObjects, AddrTablesBCU2* addrTables, PropertiesBCU2* properties);
	~BCU2() = default;

    /**
     * Begin using the EIB bus coupling unit, and set the manufacturer-ID, device type,
     * program version and a optional read-only CommObjectTable address which
     * can't be changed by ETS/KNX telegrams
     *
     * @param manufacturer - the manufacturer ID (16 bit)
     * @param deviceType - the device type (16 bit)
     * @param version - the version of the application program (8 bit)
     * @param readOnlyCommObjectTableAddress - optional (16bit), to set a read-only CommObjectTable address which won't be changed by KNX telegrams
     *                                         This is a workaround for ETS product databases, which send the "wrong" ComObjectTable address.
     *                                         Compare inside the extracted *.knxprod product database M-xxxx_x_xxxx-xx-xxxx.xml
     *                                         node <KNX/ManufacturerData/Manufacturer/ApplicationPrograms/ApplicationProgram/Static/ComObjectTable CodeSegment="M-xxxx_x-xxxx-xx-xxxx_xx-HHHH"
     *                                         HHHH = communication object table address in hexadecimal
     *                                         and
     *                                         node <KNX/ManufacturerData/Manufacturer/ApplicationPrograms/ApplicationProgram/Static/LoadProcedures/LdCtrlTaskSegment LsmIdx="3" Address="dddddd" />
     *                                         dddddd = communication object table address in decimal ETS wants us to use
     *                                         convert dddddd to hexadecimal => WWWW
     *                                         in case HHHH != WWWW ,
     *                                         use bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION, 0xHHHH) to set the correct read-only ComObjectTable address (HHHH)
     */
	virtual void begin(int manufacturer, int deviceType, int version, word readOnlyCommObjectTableAddress);

    /**
     * Begin using the EIB bus coupling unit, and set the manufacturer-ID, device type,
     * program version and a optional read-only CommObjectTable address which
     * can't be changed by ETS/KNX telegrams
     *
     * @param manufacturer - the manufacturer ID (16 bit)
     * @param deviceType - the device type (16 bit)
     * @param version - the version of the application program (8 bit)
     */
    virtual void begin(int manufacturer, int deviceType, int version) override;

    virtual bool applicationRunning() const;

    /**
     * Set our own physical address. Normally the physical address is set by ETS when
     * programming the device.
     *
     * @param addr - the physical address
     */
    void setOwnAddress(uint16_t addr) override;

    /**
     * Process a broadcast telegram.
     */
    bool processBroadCastTelegram(ApciCommand apciCmd, unsigned char *telegram, uint8_t telLength) override;

    //  BCU 2, mask version 2.0
    virtual const char* getBcuType() const override { return "BCU2"; }
    virtual unsigned short getMaskVersion() const override { return 0x20; }

    /**
     * Get the read-only CommObjectTable address, which can be set calling Begin(...)
     * @return The read-only CommObjectTable address which can't be changed by KNX telegrams
     */
    word getCommObjectTableAddressStatic() const;

    /** Start address of the user RAM when ETS talks with us. */
    const int userRamStartDefault = 0;

    /** The size of the user RAM in bytes. */
    const int userRamSize = 0x100;
    /** How many bytes have to be allocated at the end of the RAM
    	for shadowed values
    */
    const int userRamShadowSize = 0;

    /** The start of the HighRam, will be ignored by us, its only needed for device programming */
    const int highRamStart = 0x0900;

    /** The length of the HighRam */
    const int highRamLength = 0xBC;

    /**
     * Set the hardware type (Object ID 0, Property 78 (PID_HARDWARE_TYPE) formerly done by memcpy(userEeprom.order()...)
     *
     * @param hardwareType  Buffer containing the hardware type
     * @param size          Size of the provided buffer
     */
    virtual void setHardwareType(const byte* hardwareType, uint8_t size);

	UserRamBCU2* userRam;

	UserEepromBCU2* userEeprom;

	PropertiesBCU2* properties;

protected:
    bool processApci(ApciCommand apciCmd, const uint16_t senderAddr, const int8_t senderSeqNo,
            unsigned char *telegram, uint8_t telLength) override;

    void sendApciIndividualAddressSerialNumberReadResponse();

    word commObjectTableAddressStatic;       //!> The read-only CommObjectTable address which can't be changed by KNX telegrams
};

#endif /*sblib_bcu2_h*/
