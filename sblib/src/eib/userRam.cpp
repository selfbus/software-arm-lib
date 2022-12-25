/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Main Group Description
 * @defgroup SBLIB_SUB_GROUP_1 Sub Group 1 Description
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   userRam.cpp
 * @author Darthyson <darth@maptrack.de>
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/
#include <stdint.h>
#include <sblib/eib/userRam.h>
#include <sblib/bits.h>
#include <cstring>

UserRam::UserRam(uint32_t start, uint32_t size, uint32_t shadowSize) :
        Memory(start, size + shadowSize),
        userRamData(new uint8_t[size + shadowSize]()),
        _status(0),
        _runState(0),
        shadowSize(shadowSize)
{
}

uint8_t& UserRam::status()
{
    return (_status);
}

uint8_t& UserRam::runState()
{
    return (_runState);
}

bool UserRam::isStatusAddress(uint32_t address) const
{
    ///\todo check the real location of status for a BIM112, is it also 0x60 like for a bcu1 and 2?
    //return ((address - startAddress) == statusOffset());
    return (address == statusOffset());
}

void UserRam::cpyToUserRam(uint32_t address, unsigned char * buffer, uint32_t count)
{
    ///\todo check the real location of status for a BIM112, is it also 0x60 like for a bcu1 and 2?
    if ((address == statusOffset()) && (count == 1))
    {
        _status = *buffer;
        return;
    }

    normalizeAddress(&address);
    if ((address > statusOffset()) || ((address + count) < statusOffset()))
    {
        memcpy(userRamData + address, buffer, count);
    }
    else
    {
        while (count--)
        {
            if (address == statusOffset())
                _status = * buffer;
            else
                userRamData[address] = * buffer;
            buffer++;
            address++;
        }
    }
}

void UserRam::cpyFromUserRam(uint32_t address, unsigned char * buffer, uint32_t count)
{
    ///\todo check the real location of status for a BIM112, is it also 0x60 like for a bcu1 and 2?
    if ((address == statusOffset()) && (count == 1))
    {
        *buffer = _status;
        return;
    }

    normalizeAddress(&address);
    if ((address > statusOffset()) || ((address + count) < statusOffset()))
    {
        memcpy(buffer, userRamData + address, count);
    }
    else
    {
        while (count--)
        {
            if (address == statusOffset())
                * buffer = _status;
            else
                * buffer = userRamData[address];
            buffer++;
            address++;
        }
    }
}

void UserRam::setUserRamStart(const uint32_t &newRamStart)
{
    startAddress = newRamStart;
    endAddress = startAddress + sizeTotal - 1;
}

uint8_t& UserRam::operator[](uint32_t address)
{
    normalizeAddress(&address);
    return userRamData[address];
}

uint8_t UserRam::getUInt8(uint32_t address) const
{
    normalizeAddress(&address);
    if (address > (size() - 1))
    {
        return 0;
    }
    return userRamData[address];
}

uint16_t UserRam::getUInt16(uint32_t address) const
{
    normalizeAddress(&address);
    if ((address + 1) > (size() - 1))
    {
        return 0;
    }
    return makeWord(userRamData[address], userRamData[address + 1]);
}


/** @}*/
