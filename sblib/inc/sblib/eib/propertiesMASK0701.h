/*
 *  properties.h - BCU 2 properties of EIB objects.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_properties_mask0701_h
#define sblib_properties_mask0701_h

#include <sblib/eib/propertiesBCU2.h>
#include <sblib/eib/userRamMASK0701.h>
#include <sblib/eib/userEepromMASK0701.h>

class MASK0701;

/** Define a PropertyDef pointer to variable v in the userRam */
#undef PD_USER_RAM_OFFSET
#define PD_USER_RAM_OFFSET(v) (UserRamMASK0701::v + PPT_USER_RAM)

/** Define a PropertyDef pointer to variable v in the userEeprom */
#undef PD_USER_EEPROM_OFFSET
#define PD_USER_EEPROM_OFFSET(v) (UserEepromMASK0701::v + PPT_USER_EEPROM)

class PropertiesMASK0701 : public PropertiesBCU2
{
public:
	PropertiesMASK0701(MASK0701* bcuInstance) : PropertiesBCU2((BCU2*)bcuInstance), bcu(bcuInstance) {};
	~PropertiesMASK0701() = default;

	virtual LoadState handleAllocAbsDataSegment(const int objectIdx, const byte* payLoad, const int len);
	virtual LoadState handleAllocAbsTaskSegment(const int objectIdx, const byte* payLoad, const int len);

private:
	MASK0701* bcu;
};


#endif /*sblib_properties_mask0701_h*/
