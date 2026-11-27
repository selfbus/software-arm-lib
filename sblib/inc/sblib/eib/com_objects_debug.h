/**
 *  com_objects_debug.h - KNX Communication objects debugging helper.
 *
 */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */


#ifndef SBLIB_EIB_COM_OBJECTS_DEBUG_H_
#define SBLIB_EIB_COM_OBJECTS_DEBUG_H_

#include <sblib/libconfig.h>
#include <sblib/utils.h>

#if defined(DUMP_COM_OBJ)
#   include <sblib/serial.h>
#endif

#ifdef DUMP_COM_OBJ
#   define d(x) x
#else
#   define d(x)
#endif


void printComObjectConfig(uint8_t config);
void printComObjectType(uint8_t type);

#endif /* SBLIB_EIB_COM_OBJECTS_DEBUG_H_ */
