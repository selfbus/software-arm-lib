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
#ifndef sblib_bcu1_h
#define sblib_bcu1_h

#include <sblib/eib/bcu_base.h>
#include <sblib/eib/userEepromBCU1.h>
#include <sblib/eib/userRamBCU1.h>
#include <sblib/eib/com_objectsBCU1.h>
#include <sblib/eib/addr_tablesBCU1.h>

/**
 * Class for controlling all BCU related things.
 *
 * In order to use the EIB bus, you need to call bcu.begin() once in your application's
 * setup() function.
 */
class BCU1 : public BcuBase
{
public:
	BCU1();
	BCU1(UserRamBCU1* userRam, UserEepromBCU1* userEeprom, ComObjectsBCU1* comObjects, AddrTablesBCU1* addrTables);
	virtual ~BCU1() = default;

    /**
     * Begin using the EIB bus coupling unit, and set the manufacturer-ID, device type,
     * program version and a optional read-only CommObjectTable address which
     * can't be changed by ETS/KNX telegrams
     *
     * @param manufacturer - the manufacturer ID (16 bit)
     * @param deviceType - the device type (16 bit)
     * @param version - the version of the application program (8 bit)
     */
    virtual void begin(int manufacturer, int deviceType, int version);

    virtual bool applicationRunning() const;

    /**
     * Set our own physical address. Normally the physical address is set by ETS when
     * programming the device.
     *
     * @param addr - the physical address
     */
    virtual void setOwnAddress(int addr);

    //  BCU 1, mask version 1.2
    virtual const char* getBcuType() const { return "BCU1"; }
    virtual const unsigned short getMaskVersion() const { return  0x12; }

    /** Start address of the user RAM when ETS talks with us. */
    const int userRamStartDefault = 0;

    /** The size of the user RAM in bytes. */
    const int userRamSize = 0x100;
    /** How many bytes have to be allocated at the end of the RAM
    	for shadowed values
    */
    const int userRamShadowSize = 3;

	UserRamBCU1* userRam;

	UserEepromBCU1* userEeprom;

	ComObjectsBCU1* comObjects;

	AddrTablesBCU1* addrTables;

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
