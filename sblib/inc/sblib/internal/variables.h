/*
 *  internal/variables.h - Library internal shared variables
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_internal_variables_h
#define sblib_internal_variables_h

// System time in milliseconds (from timer.cpp)
extern volatile unsigned int systemTime;

#if 0
#if BCU_TYPE != BCU1_TYPE
// Table of interface/property objects (eib/property_defs.cpp)
extern const PropertyDef* const propertiesTab[NUM_PROP_OBJECTS];

#endif
#endif

#endif /*sblib_internal_variables_h*/
