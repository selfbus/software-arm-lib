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
//byte __attribute__ ((aligned (4))) userRamPadding[20];

volatile byte userEepromModified;
volatile unsigned int writeUserEepromTime;
int userRamStart = USER_RAM_START_DEFAULT;
#endif
