/*
 *  bh1750.cpp - BH1750 - Ambient Light Sensor IC for (I²C).
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/core.h>
#include <sblib/types.h>
#include <sblib/i2c.h>

#include <sblib/i2c/bh1750.h>

I2C *i2c_bh17;

/*****************************************************************************
** Function name:  BH1750Init
**
** Descriptions:   Initialize the BH1750 (power on then reset)
**
** parameters:     none
**
** Returned value: true on success, false on failure
**
*****************************************************************************/
bool BH1750::BH1750Init()
{
  i2c_bh17= I2C::Instance();
  if(!i2c_bh17->bI2CIsInitialized) {
      i2c_bh17->I2CInit();
  }

  this->bBH1750InitState= false;
  this->uLuxCurrent= 0;

  uint8_t uDataPowerOn[1]; uDataPowerOn[0] = BH1750_POWER_ON;
  uint8_t uDataReset[1]; uDataReset[0] = BH1750_RESET;
  this->bBH1750InitState= i2c_bh17->Write(BH17_ADDR, (const char*) uDataPowerOn, 1) &&  // 1. Send (Write): Power ON
                          i2c_bh17->Write(BH17_ADDR, (const char*) uDataReset, 1);      // 2. Send (Write): Reset
  return this->bBH1750InitState;
}

/*****************************************************************************
** Function name:  BH1750DeInit
**
** Descriptions:   DeInitialize the BH1750 (power down the bh1750)
**
** parameters:     None
** Returned value: None
**
*****************************************************************************/
void BH1750::BH1750DeInit()
{
  this->uLuxCurrent= 0;
  this->PowerDownBH1750();
  this->bBH1750InitState= false;
}

/*****************************************************************************
** Function name:  PowerDownBH1750
**
** Descriptions:   Power down the BH1750
**
** parameters:     none
**
** Returned value: true on success, false on failure
**
*****************************************************************************/
bool BH1750::PowerDownBH1750()
{
  uint8_t uDataPowerOn[1];
  uDataPowerOn[0] = BH1750_POWER_DOWN;
  return i2c_bh17->Write(BH17_ADDR, (const char*) uDataPowerOn, 1); // Send (Write): Power DOWN
}

/*****************************************************************************
** Function name:  GetLux
**
** Descriptions:   Read the LUX Value from BH1750
**
** parameters:     [out] Lux
**
** Returned value: true on success, false on failure
**
*****************************************************************************/
bool BH1750::GetLux()
{
  bool bRet= false;
  uint8_t uSendData[2];
  uSendData[0] = BH17_CONFIG;
  if( i2c_bh17->Write(BH17_ADDR, (const char*)uSendData, 1) ) // Send (Write): Configure resolution_Mode register before reading
  {
    uSendData[0] = 0x00;                                  // Start address to read the lux value
    if(i2c_bh17->Read(BH17_ADDR, (char*)uSendData, 2, 200 ) )
    {
      this->uLuxCurrent= ( ( uSendData[0] << 8) + uSendData[1] );// / 2;
      bRet=true;
    }
  }
  return bRet;
}
