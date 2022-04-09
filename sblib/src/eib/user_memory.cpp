/*
 *  user_memory.cpp - BCU user RAM and user EEPROM.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#if 0
//#include <sblib/eib/user_memory.h>

#include <sblib/internal/iap.h>
#include <sblib/eib/bus.h>
#include <sblib/core.h>
#include <sblib/eib/userRam.h>

#include <string.h>

// move userRamData to a nice address (ok it's stupid but it helps debugging)
//byte __attribute__ ((aligned (FLASH_RAM_BUFFER_ALIGNMENT))) userRamPadding[20];

byte  __attribute__ ((aligned (FLASH_RAM_BUFFER_ALIGNMENT))) userRamData[USER_RAM_SIZE+USER_RAM_SHADOW_SIZE];
UserRam& userRam = *(UserRam*) userRamData;
// TODO: the line above causes a compiler warning which should be avoided:
// warning : dereferencing type-punned pointer will break strict-aliasing rules [-Wstrict-aliasing]

byte  __attribute__ ((aligned (FLASH_RAM_BUFFER_ALIGNMENT))) userEepromData[USER_EEPROM_SIZE];
UserEeprom& userEeprom = *(UserEeprom*) userEepromData;
// TODO: the line above causes a compiler warning which should be avoided:
// warning : dereferencing type-punned pointer will break strict-aliasing rules [-Wstrict-aliasing]

volatile byte userEepromModified;
volatile unsigned int writeUserEepromTime;
int userRamStart = USER_RAM_START_DEFAULT;
#endif
