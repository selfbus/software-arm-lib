/*
 *  types.h - EIB data types
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_eib_types_bcu1_h
#define sblib_eib_types_bcu1_h

#include <sblib/eib/types.h>

/**
 * A communication object configuration.
 */
struct ComConfigBCU1 : public ComConfig
{
	/**
	 * Depending on the BCU type define the size of the pointer to the RAM
	 * location of the value for a com object
	 */
	typedef byte DataPtrType;

	/** Data pointer, low byte. Depending on the COMCONF_VALUE_TYPE flag in the
     * config byte, this pointer points to userRam or userEeprom.
     */
    DataPtrType dataPtr;
};

#endif /*sblib_eib_types_h*/
