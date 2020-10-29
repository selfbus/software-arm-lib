/*
 * Library to connect a Air Quality Sensor IAQ-Core from AMS
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 *
 *  Adapted to Selfbus Library by Oliver Stefan (2019)
 */

#ifndef IAQ_CORE_H
#define IAQ_CORE_H

class IAQcoreClass{
private:
	typedef enum {
	    IAQcoreAddress = 0xB4,
	} HUM_SENSOR_T;
public:
	bool Init(void);
	bool GetIAQ(uint16_t &CO2eq, uint8_t &status, uint32_t &resistance, uint16_t &Tvoc);
	bool GetIAQco2(uint16_t &CO2eq);
};

#endif
