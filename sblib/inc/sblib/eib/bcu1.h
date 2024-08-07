/*
 *  bcu1.h - BCU specific stuff.
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

#include <sblib/eib/bcu_default.h>
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
class BCU1 : public BcuDefault
{
public:
    BCU1();
    BCU1(UserRamBCU1* userRam, UserEepromBCU1* userEeprom, ComObjectsBCU1* comObjects, AddrTablesBCU1* addrTables);
    ~BCU1() = default;

    /**
     * Begin using the EIB bus coupling unit, and set the manufacturer-ID, device type, program version
     *
     * @param manufacturer - the manufacturer ID (16 bit)
     * @param deviceType - the device type (16 bit)
     * @param version - the version of the application program (8 bit)
     */
    virtual void begin(int manufacturer, int deviceType, int version) override;

    bool applicationRunning() const;

    //  BCU 1, mask version 1.2
    const char* getBcuType() const override { return "BCU1"; }
    uint16_t getMaskVersion() const override { return  0x12; }

    UserRamBCU1* userRam;

    UserEepromBCU1* userEeprom;

    ComObjectsBCU1* comObjects;

    AddrTablesBCU1* addrTables;

protected:
};

#endif /*sblib_bcu1_h*/
