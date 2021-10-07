/***************************************************
  This is a library for the CCS811 digital TVOC/eCO2 Sensor by CCMOSS/AMS
  http://www.ccmoss.com/gas-sensors#CCS811

  Updated: June 17, 2017

  The sensor uses I2C protocol to communicate, and requires 2 pins - SDA and SCL
  Another GPIO is also required to assert the WAKE pin for communication. this
  pin is passed by an argument in the begin function.

  A breakout board is available: https://github.com/AKstudios/CCS811-Breakout

  The ADDR pin on the sensor can be connected to VCC to set the address as 0x5A.
  The ADDR pin on the sensor can be connected to GND to set the address as 0x5B.

  Written by Akram Ali from AKstudios (www.akstudios.com)
  GitHub: https://github.com/AKstudios/

  BSD license, all text above must be included in any redistribution

  Adapted to Selfbus Library by Oliver Stefan (2018)
   Adapted to lpcopen I2C functions by Oliver Stefan (2021)
 ****************************************************/


#include <stdint.h>

#ifndef CCS811_h
#define CCS811_h

#define nWAKE           4      // pin 4 is tied to CCS811 nWAKE pin in sensor node
#define nINT            7      // pin 7 is tied to CCS811 nINT pin in sensor node

#define CCS_811_ADDR            0x5A   // when I2C_ADDR pin is LOW
//#define CCS_811_ADDR            0x5B   // when I2C_ADDR pin is HIGH

const uint8_t STATUS = 		0x00;
const uint8_t MEAS_MODE =     	0x01;
const uint8_t ALG_RESULT_DATA =0x02;
const uint8_t ENV_DATA =		0x05;
const uint8_t APP_START =     	0xF4;
const uint8_t HW_ID =	        0x20;
const uint8_t ERROR_ID =       0xE0;
const uint8_t SW_RESET =       0xFF;

class CCS811Class
{
public:
  CCS811Class();     // constructor
  bool begin(uint8_t I2C_ADDR, int WAKE_PIN);
  char readStatus(void);
  char readHW_ID(void);
  char readErrorID(char _status);
  int readTVOC(void);
  int readCO2(void);
  void getData(void);
  void compensate(float t, float rh);
  void _digitalWrite(int WAKE_PIN, bool VAL);
  void reset(void);
  void sleep();
  int TVOC, CO2;

private:
  int _WAKE_PIN;
  uint8_t _I2C_ADDR;
};

#endif
