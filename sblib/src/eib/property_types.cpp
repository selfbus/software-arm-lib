/*
 *  property_types.cpp - BCU 2 property types of EIB objects.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/property_types.h>
#include <sblib/eib/bcu_default.h>

#if defined(INCLUDE_SERIAL)
#   include <sblib/serial.h>
#endif

/**
 * PropertyDataType sizes in bytes
 */
const byte propertySizes[] =
{
    1, 1, 1, 2, 2, 2, 3, 3, 4, 4, // PDT_CONTROL -> PDT_UNSIGNED_LONG
    4, 8, 10, 3, 5, 8, 0, // PDT_FLOAT - >PDT_VARIABLE_LENGTH
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, // PDT_GENERIC_01 -> PDT_GENERIC_20
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // PDT_RESERVED_0x25 -> PDT_RESERVED_0x2e
    0, 2, 6, 1, 1, 2, 1, 1, // PDT_UTF_8 -> PDT_SCALING
    0, 0, 0, 0, 0, // PDT_RESERVED_0x37 -> PDT_RESERVED_0x3b
    0, 0, 0, 0 // PDT_NE_VL -> PDT_ESCAPE
};


int PropertyDef::size() const
{
    extern const byte propertySizes[];
    return propertySizes[control & PC_TYPE_MASK];
}

byte* PropertyDef::valuePointer(BcuBase* bcu) const
{
    if (control & PC_POINTER)
    {
        int offs = valAddr & PPT_OFFSET_MASK;

        switch (valAddr & PPT_MASK)
        {
        case PPT_USER_RAM:
            DB_PROPERTIES(serial.println("RAM"););
            return bcu->userRam->userRamData + offs; ///\todo check this was correct replaced or not
        case PPT_USER_EEPROM:
            DB_PROPERTIES(serial.println("EEPROM"););
            return ((BcuDefault*)bcu)->userEeprom->userEepromData + offs;
        default:
            fatalError(); // invalid property pointer type encountered
            break;
        }
    }

    return (byte*) &valAddr;
}
