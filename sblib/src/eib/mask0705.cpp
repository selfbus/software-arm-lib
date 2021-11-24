/*
 * mask0701.cpp
 *
 *  Created on: 20.11.2021
 *      Author: dridders
 */

#include <sblib/eib/mask0705.h>
#include <sblib/eib/propertiesMASK0705.h>

MASK0705::MASK0705() : MASK0705(new UserRamMASK0705(), new UserEepromMASK0705(bus), new ComObjectsMASK0705(this), new AddrTablesMASK0705(this), new PropertiesMASK0705(this))
{}

MASK0705::MASK0705(UserRamMASK0705* userRam, UserEepromMASK0705* userEeprom, ComObjectsMASK0705* comObjects, AddrTablesMASK0705* addrTables, PropertiesMASK0705* properties) :
		MASK0701(userRam, userEeprom, comObjects, addrTables, properties)/*,
		userRam(userRam),
		userEeprom(userEeprom),
		comObjects(comObjects),
		addrTables(addrTables)*/
{}

