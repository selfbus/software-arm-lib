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
#ifndef sblib_mask0701_h
#define sblib_mask0701_h

#include <sblib/eib/bcu2.h>
#include <sblib/eib/userEepromMASK0701.h>
#include <sblib/eib/userRamMASK0701.h>
#include <sblib/eib/com_objectsMASK0701.h>
#include <sblib/eib/addr_tablesMASK0701.h>
#include <sblib/eib/propertiesMASK0701.h>

/**
 * Class for controlling all BCU related things.
 *
 * In order to use the EIB bus, you need to call bcu.begin() once in your application's
 * setup() function.
 */
class MASK0701 : public BCU2
{
public:
	MASK0701();
	MASK0701(UserRamMASK0701* userRam, UserEepromMASK0701* userEeprom, ComObjectsMASK0701* comObjects, AddrTablesMASK0701* addrTables, PropertiesMASK0701* properties);
	virtual ~MASK0701() = default;

    virtual const char* getBcuType() const { return "BIM112"; }
    virtual const unsigned short getMaskVersion() const { return 0x701; }

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

/** Address for load control */
//#   define LOAD_CONTROL_ADDR 0x104

/** Address for load state */
//#   define LOAD_STATE_ADDR 0xb6e9
};

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /*sblib_bcu_h*/
