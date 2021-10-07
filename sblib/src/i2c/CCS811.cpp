/***************************************************
 This is a library for the CCS811 digital TVOC/eCO2 Sensor by CCMOSS/AMS
 http://www.ccmoss.com/gas-sensors#CCS811

 Updated: July 13, 2017

 The sensor uses I2C protocol to communicate, and requires 2 pins - SDA and SCL
 Another GPIO is also required to assert the WAKE pin for communication. this
 pin is passed by an argument in the begin function.

 A breakout board is available: https://github.com/AKstudios/CCS811Class-Breakout

 The ADDR pin on the sensor can be connected to VCC to set the address as 0x5A.
 The ADDR pin on the sensor can be connected to GND to set the address as 0x5B.

 Written by Akram Ali from AKstudios (www.akstudios.com)
 GitHub: https://github.com/AKstudios/

 BSD license, all text above must be included in any redistribution

 Adapted to Selfbus Library by Oliver Stefan (2018)
 Adapted to lpcopen I2C functions by Oliver Stefan (2021)
 ****************************************************/

#include <sblib/i2c/CCS811.h>
#include <sblib/i2c.h>
#include <sblib/timer.h>
#include <sblib/digital_pin.h>


CCS811Class::CCS811Class() {
  // empty constructor, just because.
}

bool CCS811Class::begin(uint8_t I2C_ADDR, int WAKE_PIN) {
  delay(70); // from datasheet - up to 70ms on the first Reset after new application download; up to 20ms delay after power on
  _I2C_ADDR = I2C_ADDR;
  _WAKE_PIN = WAKE_PIN;
  i2c_lpcopen_init();
  pinMode(_WAKE_PIN, OUTPUT);   // set WAKE pin as OUTPUT

  byte hw_id = readHW_ID();
  if (hw_id != 0x81)  // this is the expected hardware ID
  {
    return false;
  }

  byte status = readStatus();
  uint8_t bit = (status & (1 << 4)) != 0; // black magic to read APP_VALID bit from STATUS register
  if (bit != 1)
  {
    readErrorID(status);
    return false;
  }
  delayMicroseconds(5); //take a little time for Wake recognizing (without this time it fails)
  _digitalWrite(_WAKE_PIN, false);
  delayMicroseconds(50); // recommended 50us delay after asserting WAKE pin
  Chip_I2C_MasterSend(I2C0, _I2C_ADDR, &APP_START, sizeof(APP_START));
  //i2c_CCS811->Write(_I2C_ADDR, &APP_START, sizeof(APP_START)); //F4

  digitalWrite(_WAKE_PIN, true);

  status = readStatus();
  bit = (status & (1 << 7)) != 0; // black magic to read FW_MODE bit from STATUS register
  if (bit != 1)
  {
    readErrorID(status);
    return false;
  }

  _digitalWrite(_WAKE_PIN, false);
  delayMicroseconds(50); // recommended 50us delay after asserting WAKE pin
  uint8_t i2cData[2] = {MEAS_MODE, 0x20};
  //i2c_CCS811->Write(_I2C_ADDR, i2cData, sizeof(i2cData));
  Chip_I2C_MasterSend(I2C0, _I2C_ADDR, i2cData, sizeof(i2cData));
  digitalWrite(_WAKE_PIN, true);

  return true;
}

char CCS811Class::readStatus(void) {
  delayMicroseconds(20); // recommended 20us delay while performing back to back I2C operations
  _digitalWrite(_WAKE_PIN, false);
  delayMicroseconds(50); // recommended 50us delay after asserting WAKE pin
  uint8_t status;// = STATUS;
  //i2c_CCS811->Read(_I2C_ADDR, &status, 1);
  Chip_I2C_MasterCmdRead(I2C0, _I2C_ADDR, STATUS, &status, 1);
  digitalWrite(_WAKE_PIN, true);
  return status;
}

char CCS811Class::readHW_ID(void) {
  _digitalWrite(_WAKE_PIN, false);
  delayMicroseconds(50); // recommended 50us delay after asserting WAKE pin
  uint8_t hw_id;// = HW_ID;
  //if( i2c_CCS811->Read(_I2C_ADDR, &hw_id, 1) != true){
  if(Chip_I2C_MasterCmdRead(I2C0, _I2C_ADDR, HW_ID, &hw_id, 1) != true){
    hw_id=0;
  }
  digitalWrite(_WAKE_PIN, true);
  return hw_id;
}

char CCS811Class::readErrorID(char _status) {
  _digitalWrite(_WAKE_PIN, false);
  delayMicroseconds(50); // recommended 50us delay after asserting WAKE pin
  uint8_t error_id;// = ERROR_ID;
  //i2c_CCS811->Read(_I2C_ADDR, &error_id, 1);
  Chip_I2C_MasterCmdRead(I2C0, _I2C_ADDR, ERROR_ID, &error_id, 1);
  digitalWrite(_WAKE_PIN, true);
  uint8_t bit = (_status & 1) != 0; // black magic to read ERROR bit from STATUS register
  if (bit)
        return error_id;
    else
        return 0;
}

void CCS811Class::sleep() {
  _digitalWrite(_WAKE_PIN, false);
  delayMicroseconds(50); // recommended 50us delay after asserting WAKE pin
  uint8_t i2cData[] = {MEAS_MODE, 0x00};
  //i2c_CCS811->Write(_I2C_ADDR, i2cData, sizeof(i2cData));
  Chip_I2C_MasterSend(I2C0, _I2C_ADDR, i2cData, sizeof(i2cData));
  digitalWrite(_WAKE_PIN, true); // set WAKE_PIN high - this puts sensor in sleep mode (~2uA) and all I2C communications are ignored
}

void CCS811Class::getData(void) {
  _digitalWrite(_WAKE_PIN, false);
  delayMicroseconds(50); // recommended 50us delay after asserting WAKE pin
  uint8_t buffer[4];
  //buffer[0] = ALG_RESULT_DATA;  // reading ALG_RESULT_DATA clears DATA_READY bit in 0x00
  //i2c_CCS811->Read(_I2C_ADDR, buffer, 4);
  Chip_I2C_MasterCmdRead(I2C0, _I2C_ADDR, ALG_RESULT_DATA, buffer, 4);

  CO2 = ((uint8_t) buffer[0] << 8) + buffer[1];
  TVOC = ((uint8_t) buffer[2] << 8) + buffer[3];

  digitalWrite(_WAKE_PIN, true);
}

int CCS811Class::readTVOC(void) {
  return TVOC;
}

int CCS811Class::readCO2(void) {
  return CO2;
}

void CCS811Class::compensate(float t, float rh)    // compensate for temperature and relative humidity
{
  _digitalWrite(_WAKE_PIN, false);
  delayMicroseconds(50); // recommended 50us delay after asserting WAKE pin
  int _temp = 0;
  int _rh;
  if (t > 0)
    _temp = (int) t + 0.5;  // this will round off the floating point to the nearest integer value
  else if (t < 0) // account for negative temperatures
    _temp = (int) t - 0.5;
  _temp = _temp + 25; // temperature high byte is stored as T+25°C in the sensor's memory so the value of byte is positive
  _rh = (int) rh + 0.5;  // this will round off the floating point to the nearest integer value

  uint8_t _ENV_DATA[5];

  _ENV_DATA[0] = ENV_DATA;
  _ENV_DATA[1] = _rh << 1;  // shift the binary number to left by 1. This is stored as a 7-bit value
  _ENV_DATA[2] = 0; // most significant fractional bit. Using 0 here - gives us accuracy of +/-1%. Current firmware (2016) only supports fractional increments of 0.5
  _ENV_DATA[3] = _temp << 1;
  _ENV_DATA[4] = 0;

  //i2c_CCS811->Write(_I2C_ADDR, _ENV_DATA, sizeof(_ENV_DATA));
  Chip_I2C_MasterSend(I2C0,_I2C_ADDR, _ENV_DATA, sizeof(_ENV_DATA));

  digitalWrite(_WAKE_PIN, true);
}

void CCS811Class::_digitalWrite(int WAKE_PIN, bool VAL) // asserts WAKE pin with a small delay to ensure reliable communication - thanks to djdehaan for this fix
{
  digitalWrite(WAKE_PIN, VAL);
  delayMicroseconds(60);
}
