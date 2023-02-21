/*
 *  addr_tables.h - BCU communication address tables.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_addr_tables_MASK0701_h
#define sblib_addr_tables_MASK0701_h

#include <sblib/eib/addr_tablesBCU2.h>

class MASK0701;
class BCU2;

class AddrTablesMASK0701 : public AddrTablesBCU2
{
public:
	AddrTablesMASK0701(MASK0701* bcuInstance) : AddrTablesBCU2((BCU2*)bcuInstance) {}
	~AddrTablesMASK0701() = default;
};

#endif /*sblib_addr_tables_MASK0701_h*/
