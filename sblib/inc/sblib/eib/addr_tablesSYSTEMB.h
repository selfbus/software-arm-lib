/*
 *  addr_tables.h - BCU communication address tables.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_addr_tables_SYSTEMB_h
#define sblib_addr_tables_SYSTEMB_h

#include <sblib/eib/addr_tablesMASK0701.h>

class SYSTEMB;

class AddrTablesSYSTEMB : public AddrTablesMASK0701
{
public:
	AddrTablesSYSTEMB(SYSTEMB* bcuInstance) : AddrTablesMASK0701((MASK0701*)bcuInstance), bcu(bcuInstance) {};
	~AddrTablesSYSTEMB() = default;

	/**
	 * Get the index of a group address in the address table.
	 *
	 * @param addr - the address to find.
	 * @return The index of the address, -1 if not found.
	 *
	 * @brief The address table contains the configured group addresses and our
	 * own physical address. This function skips the own physical address and
	 * only scans the group addresses.
	 */
	int indexOfAddr(int addr) override;
private:
	SYSTEMB* bcu;
};

#endif /*sblib_addr_tables_SYSTEMB_h*/
