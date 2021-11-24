/*
 * mask0701.cpp
 *
 *  Created on: 20.11.2021
 *      Author: dridders
 */

#include <sblib/eib/mask0701.h>
#include <sblib/eib/propertiesMASK0701.h>

MASK0701::MASK0701() : MASK0701(new UserRamMASK0701(), new UserEepromMASK0701(bus), new ComObjectsMASK0701(this), new AddrTablesMASK0701(this), new PropertiesMASK0701(this))
{}

MASK0701::MASK0701(UserRamMASK0701* userRam, UserEepromMASK0701* userEeprom, ComObjectsMASK0701* comObjects, AddrTablesMASK0701* addrTables, PropertiesMASK0701* properties) :
		BCU2(userRam, userEeprom, comObjects, addrTables, properties)/*,
		userRam(userRam),
		userEeprom(userEeprom),
		comObjects(comObjects),
		addrTables(addrTables)*/
{}
