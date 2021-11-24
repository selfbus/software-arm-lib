/*
 *  addr_tables.h - BCU communication address tables.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_addr_tables_BCU1_h
#define sblib_addr_tables_BCU1_h

#include <sblib/eib/addr_tables.h>

class BCU1;

class AddrTablesBCU1 : public AddrTables
{
public:
	AddrTablesBCU1(BCU1* bcuInstance) : bcu(bcuInstance) {};
	~AddrTablesBCU1() = default;

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
	virtual int indexOfAddr(int addr);

	/**
	 * Get the address table. The address table contains the configured group addresses
	 * and our own physical address.
	 *
	 * @return The pointer to the address table.
	 *
	 * @brief The first byte of the table contains the number of entries. The rest of
	 * the table consists of the addresses: 2 bytes per address.
	 */
	virtual byte* addrTable();

	/**
	 * Get the association table. The association table connects group addresses
	 * with communication objects.
	 *
	 * @return The pointer to the association table.
	 *
	 * @brief The first byte of the table contains the number of entries. The rest of
	 * the table consists of the associations - 2 bytes per association:
	 * 1 byte addr-table index, 1 byte com-object number.
	 */
	virtual byte* assocTable();

private:
	BCU1* bcu;
};

#endif /*sblib_addr_tables_h*/
