/*
 *  properties.h - BCU 2 properties of EIB objects.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_properties_bcu0701_h
#define sblib_properties_bcu0701_h

#include <sblib/eib/propertiesBCU2.h>

class BCU2;
class SYSTEMB;

class PropertiesSYSTEMB : public PropertiesBCU2
{
public:
	PropertiesSYSTEMB(SYSTEMB* bcuInstance) : PropertiesBCU2((BCU2*)bcuInstance), bcu(bcuInstance) {};
	~PropertiesSYSTEMB() = default;

	virtual LoadState handleAllocAbsDataSegment(const int objectIdx, const byte* payLoad, const int len);
	virtual LoadState handleDataRelativeAllocation(const int objectIdx, const byte* payLoad, const int len);
	virtual uint16_t crc16(uint8_t* ptr, int len);
	virtual int loadProperty(int objectIdx, const byte* data, int len);
	virtual bool propertyValueReadTelegram(int objectIdx, PropertyID propertyId, int count, int start);
	virtual bool propertyValueWriteTelegram(int objectIdx, PropertyID propertyId, int count, int start);

private:
	SYSTEMB* bcu;
};
#endif /*sblib_properties_h*/
