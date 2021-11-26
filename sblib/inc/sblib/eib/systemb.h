/*
 *  bcu.h - BCU specific stuff.
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
#ifndef sblib_systemb_h
#define sblib_systemb_h

#include <sblib/eib/bcu2.h>
#include <sblib/eib/userEepromSYSTEMB.h>
#include <sblib/eib/userRamSYSTEMB.h>
#include <sblib/eib/com_objectsSYSTEMB.h>
#include <sblib/eib/propertiesSYSTEMB.h>
#include <sblib/eib/addr_tablesSYSTEMB.h>

/**
 * Class for controlling all BCU related things.
 *
 * In order to use the EIB bus, you need to call bcu.begin() once in your application's
 * setup() function.
 */
class SYSTEMB : public BCU2
{
public:
	SYSTEMB();
	SYSTEMB(UserRamSYSTEMB* userRam, UserEepromSYSTEMB* userEeprom, ComObjectsSYSTEMB* comObjects, AddrTablesSYSTEMB* addrTables, PropertiesSYSTEMB* properties);
	virtual ~SYSTEMB() = default;

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
    virtual void begin(int manufacturer, int deviceType, int version, word readOnlyCommObjectTableAddress = 0);

    virtual bool applicationRunning() const;

    /**
     * Set our own physical address. Normally the physical address is set by ETS when
     * programming the device.
     *
     * @param addr - the physical address
     */
    virtual void setOwnAddress(int addr);

    virtual const char* getBcuType() const { return "SYSTEM_B"; }
    virtual const unsigned short getMaskVersion() { return  0x7B0; }

    virtual bool processApciMemoryReadPDU(int addressStart, byte *payLoad, int lengthPayLoad);
    virtual bool processApciMemoryWritePDU(int addressStart, byte *payLoad, int lengthPayLoad);

    /** Start address of the user RAM when ETS talks with us. */
    const int userRamStartDefault = 0x5FC;

    const int extraUserRamSize = 0;

    /** The size of the user RAM in bytes. */
    const int userRamSize = (0x304 + extraUserRamSize);
    /** How many bytes have to be allocated at the end of the RAM
    	for shadowed values
    */
    const int userRamShadowSize = 3;

    /** The start of the HighRam, will be ignored by us, its only needed for device programming */
    const int highRamStart = 0x0900;

    /** The length of the HighRam */
    const int highRamLength = 0xBC;

/** Address for load control */
//#   define LOAD_CONTROL_ADDR 0x104

/** Address for load state */
//#   define LOAD_STATE_ADDR 0xb6e9

	UserEepromSYSTEMB* userEeprom;

protected:
    // The method begin_BCU() is renamed during compilation to indicate the BCU type.
    // If you get a link error then the library's BCU_TYPE is different from your application's BCU_TYPE.
    virtual void begin_BCU(int manufacturer, int deviceType, int version);

    virtual void processDirectTelegram(int apci);
};

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /*sblib_bcu_h*/
