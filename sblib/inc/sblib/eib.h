/*
 *  eib.h - Include the EIB / BCU related header files.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_eib_h
#define sblib_eib_h

#ifndef NO_OOP_MACROS

#include <sblib/core.h>

#include <sblib/eib/bus.h>
#include <sblib/eib/addr_tables.h>
#include <sblib/eib/bcu_base.h>
#include <sblib/types.h>


#if defined(BCU2) || BCU_TYPE == 0x20 || BCU_TYPE == 20
	#include <sblib/eib/bcu2.h>
	#define setup() innerSetup(); BCU2 bcu = BCU2(); BcuBase* setup() { innerSetup(); return &bcu; } void innerSetup()
	extern BCU2 bcu;
#elif defined(BIM112) || defined(BIM112_71) || BCU_TYPE == 0x701
	#include <sblib/eib/mask0701.h>
	#define setup() innerSetup(); MASK0701 bcu = MASK0701(); BcuBase* setup() { innerSetup(); return &bcu; } void innerSetup()
	extern MASK0701 bcu;
#elif defined(BIM112_75) || BCU_TYPE == 0x705
	#include <sblib/eib/mask0705.h>
	#define setup() innerSetup(); MASK0705 bcu = MASK0705(); BcuBase* setup() { innerSetup(); return &bcu; } void innerSetup()
	extern MASK0705 bcu;
#elif defined(SYSTEM_B) || BCU_TYPE == 0x7B0
	#include <sblib/eib/systemb.h>
	#define setup() innerSetup(); SYSTEMB bcu = SYSTEMB(); BcuBase* setup() { innerSetup(); return &bcu; } void innerSetup()
	extern SYSTEMB bcu;
#elif defined(BCU1) || defined(BCU1_12) || BCU_TYPE == 0x10 || BCU_TYPE == 10 || !defined(BCU_TYPE)
	#include <sblib/eib/bcu1.h>
	#define setup() innerSetup();    \
                BCU1 bcu = BCU1();   \
                BcuBase* setup()     \
                {                    \
                    innerSetup();    \
                    return &bcu;     \
                }                    \
                void innerSetup()
    #define objectConfig(x) bcu.comObjects->objectConfigBCU1(x)
	extern BCU1 bcu;
#endif

#define _bcu bcu

#define MASK_VERSION (bcu.getMaskVersion())

#define objectWrite(x, y) bcu.comObjects->objectWrite(x, y)
#define objectWriteFloat(x, y) bcu.comObjects->objectWriteFloat(x, y)
#define objectSetValue(x, y) bcu.comObjects->objectSetValue(x, y)
#define objectRead(x) bcu.comObjects->objectRead(x)
#define requestObjectRead(x) bcu.comObjects->requestObjectRead(x)
#define nextUpdatedObject() bcu.comObjects->nextUpdatedObject()
#define objectUpdate(x, y) bcu.comObjects->objectUpdate(x, y)
#define objectWritten(x) bcu.comObjects->objectWritten(x)
#define objectEndian(x) bcu.comObjects->objectEndian(x)
#define objectSize(x) bcu.comObjects->objectSize(x)


#define indexOfAddr(x) bcu.addrTables->indexOfAddr(x)

#define userEepromData userEeprom.userEepromData
#define userRamData userRam.userRamData
#define userEeprom (*(bcu.userEeprom))
#define userRam (*(bcu.userRam))
#define userMemoryPtr bcu.userMemoryPtr
#define setUserRamStart userRam.setUserRamStart

#define USER_EEPROM_START userEeprom.userEepromStart
#define USER_EEPROM_SIZE userEeprom.userEepromSize
#define USER_EEPROM_END userEeprom.userEepromEnd

#define bus (*(bcu.bus))


#endif /* NO_OOP_MACROS */
#endif /*sblib_eib_h*/
