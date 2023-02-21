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
 * @file   memory.cpp
 * @author Darthyson <darth@maptrack.de>
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/eib/memory.h>

Memory::Memory(uint32_t  start, uint32_t size):
      startAddress(start),
      endAddress(start + size - 1),
      sizeTotal(size)
{}

uint32_t Memory::startAddr() const
{
    return (startAddress);
}

uint32_t Memory::endAddr() const
{
    return (endAddress);
}

uint32_t Memory::size() const
{
    return (sizeTotal);
}

bool Memory::inRange(uint32_t address) const
{
    return ((address >= startAddr()) && (address <= endAddr()));
}

bool Memory::inRange(uint32_t start, uint32_t end) const
{
    return ((start <= end) &&
            (start >= startAddr()) &&
            (end <= endAddr()));
}




/** @}*/
