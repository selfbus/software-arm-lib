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
#ifndef sblib_mask0705_h
#define sblib_mask0705_h

#include <sblib/eib/mask0701.h>
#include <sblib/eib/userEepromMASK0705.h>
#include <sblib/eib/userRamMASK0705.h>
#include <sblib/eib/com_objectsMASK0705.h>
#include <sblib/eib/addr_tablesMASK0705.h>
#include <sblib/eib/propertiesMASK0705.h>

/**
 * Class for controlling all BCU related things.
 *
 * In order to use the EIB bus, you need to call bcu.begin() once in your application's
 * setup() function.
 */
class MASK0705 : public MASK0701
{
public:
	MASK0705();
	MASK0705(UserRamMASK0705* userRam, UserEepromMASK0705* userEeprom, ComObjectsMASK0705* comObjects, AddrTablesMASK0705* addrTables, PropertiesMASK0705* properties);
	~MASK0705() = default;

    virtual const char* getBcuType() const { return "BIM112_75"; }
    virtual const unsigned short getMaskVersion() const { return  0x705; }

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
