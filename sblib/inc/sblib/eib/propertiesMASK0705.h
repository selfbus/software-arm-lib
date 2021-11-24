/*
 *  properties.h - BCU 2 properties of EIB objects.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_properties_mask0705_h
#define sblib_properties_mask0705_h

#include <sblib/eib/propertiesMASK0701.h>

class MASK0701;
class MASK0705;

class PropertiesMASK0705 : public PropertiesMASK0701
{
public:
	PropertiesMASK0705(MASK0705* bcu) : PropertiesMASK0701((MASK0701*)bcu) {};
	~PropertiesMASK0705() = default;
};
#endif /*sblib_properties_h*/
