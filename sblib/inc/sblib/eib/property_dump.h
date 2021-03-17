/*
 *  property_dump.h - dumping to serial of BCU 2 & BIM112 properties of EIB objects.
 *
 *  Copyright (c) 2021 Darthyson <darth@maptrack.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_property_dump_h
#define sblib_property_dump_h

#include <sblib/eib/properties.h>
#include <sblib/eib/property_types.h>

#ifdef DUMP_PROPERTIES
#   include <sblib/serial.h>
#endif

#if BCU_TYPE != BCU1_TYPE

/**
 * Include the C++ code snippet if DUMP_PROPERTIES is defined, do not include the code
 * if DUMP_PROPERTIES is not defined.
 *
 * @param code - the C++ code to include
 *
 * @brief Example:  DUMP_PROPERTIES(fatalError())
 */
#ifdef DUMP_PROPERTIES
#  define IF_DUMP_PROPERTIES(code) { code; }
#else
#  define IF_DUMP_PROPERTIES(code)
#endif

#ifdef DUMP_PROPERTIES

/**
 * Prints the value (in hex) of objectIdx and its clear name
 *
 * @param objectIdx - object index to print
 */
void printObjectIdx(int objectIdx);

/**
 * Prints a loadstate (in hex) and its clear name
 *
 * @param loadstate - loadstate to print
 */
void printLoadState(int loadstate);

/**
 * Prints a segmenttype (in hex) and its clear name
 *
 * @param segmenttype - segmenttype to print
 */
void printSegmentType(int segmenttype);

#endif

#endif /*BCU_TYPE != BCU1_TYPE*/

#endif /*sblib_property_dump_h*/
