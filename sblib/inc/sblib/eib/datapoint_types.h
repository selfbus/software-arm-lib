/*
 *  datapoint_types.h - Conversion functions for datapoint types.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_datapoint_types_h
#define sblib_datapoint_types_h


/**
 * An invalid 2 byte float (DPT9/EIS5).
 * To be used for floatToDpt9() and dpt9ToFloat().
 */
#define INVALID_DPT_FLOAT  2147483647U

/**
 * Convert a "float" value to 2 byte DPT9.xxx
 * @details The possible range of the values is -67108864 to 67076096.
 *
 * @param value - the value to convert.
 *                Use @ref INVALID_DPT_FLOAT for the DPT9 "invalid data" value.
 * @return The 2 byte float (DPT9/EIS5).
 * @note KNX Spec. 2.1 3/7/2 3.10 p.32 for details on DPT9.xxx
 */
unsigned short floatToDpt9(int value);

/**
 * Convert a DPT9.xxx to integer.
 *
 * @param dptValue - the 2 byte float (DPT9/EIS5) to convert
 * @return The value as integer, or @ref INVALID_DPT_FLOAT for the
 *         DPT9 "invalid data" value.
 * @note KNX Spec. 2.1 3/7/2 3.10 p.32 for details on DPT9.xxx
 */
int dpt9ToFloat(unsigned short dptValue);


#endif /*sblib_datapoint_types_h*/
