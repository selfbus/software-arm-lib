/*
 * Library to connect a Air Quality Sensor IAQ-Core from AMS
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 *
 *  Adapted to Selfbus Library by Oliver Stefan (2019)
 */

/*
 * IAQ-CORE Funktionen werden aktuell in keinem Projekt benötigt und wurden noch nicht an die neuen I2C Funktionen angepasst.
 * Daher sind alle IAQ-CORE Funktionen vorerst deaktiviert
 *

#include <sblib/i2c.h>
#include <sblib/i2c/iaq-core.h>


I2C *i2c_iaq_core;

/ ******************************************************************************
 * Global Functions
 ******************************************************************************

/ *****************************************************************************
** Function name:  Init
**
** Descriptions:   Initialize the IAQ core
**
** parameters:     none
**
** Returned value: true on success, false on failure
**
*****************************************************************************
bool IAQcoreClass::Init(void)
{
	i2c_iaq_core= I2C::Instance();
	if(!i2c_iaq_core->bI2CIsInitialized) {
		i2c_iaq_core->I2CInit();
	}
	return (i2c_iaq_core ? true:false);
}

/ *****************************************************************************
** Function name:  GetIAQ
**
** Descriptions:   Get all values from the IAQ core sensor
**
** parameters:     	CO2eq: 		Prediction (CO2 eq. ppm)
** 					status: 	0x00: OK    (data valid)
**								0x10: RUNIN (module in warm up phase)
**								0x01: BUSY (re-read multi byte data!)
**								0x80: ERROR (if constant: replace sensor)
** 					resistance: Sensor resistance [Ohm]
** 					Tvoc:		Prediction (TVOC eq. ppb)
**
** Returned value: true on success, false on failure
**
*****************************************************************************
bool IAQcoreClass::GetIAQ(uint16_t &CO2eq, uint8_t &status, uint32_t &resistance, uint16_t &Tvoc)
{
	uint8_t result[9];
	if(i2c_iaq_core->Read(IAQcoreAddress, (char*) result, 9, 0) == false){
		i2c_iaq_core->I2CInit();
		return 0;
	}
	CO2eq = ((result[0] << 8) | (result[1] << 0));
	status = result[2];
	//Byte 3 is always 0
	resistance = ((result[3] << 24) | (result[4] << 16) | (result[5] << 8) | (result[6] << 0));
	Tvoc = ((result[7] << 8) | (result[8] << 0));
	return 1;
}

bool IAQcoreClass::GetIAQco2(uint16_t &CO2eq){
	uint8_t result[2];
	if(i2c_iaq_core->Read(IAQcoreAddress, (char*) result, 2, 0) == false){
		i2c_iaq_core->I2CInit();
		return 0;
	}
	CO2eq = ((result[0] << 8) | (result[1] << 0));
	return 1;
}

*/