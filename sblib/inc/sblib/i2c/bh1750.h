/*
 *  bh1750.h - BH1750 - Ambient Light Sensor IC for (I²C).
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef bh1750_h
#define bh1750_h

                                                  // For more information: Read BH1750 Datasheet
#define BH1750_I2CADDR                      0x23
#define BH1750_POWER_DOWN                   0x00  // No active state
#define BH1750_POWER_ON                     0x01  // Wating for measurment command
#define BH1750_RESET                        0x07  // Reset data register value - not accepted in POWER_DOWN mode

#define BH1750_CONTINUOUS_HIGH_RES_MODE     0x10  // Start measurement at 1lx resolution. Measurement time is approx 120ms.
#define BH1750_CONTINUOUS_HIGH_RES_MODE_2   0x11  // Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
#define BH1750_CONTINUOUS_LOW_RES_MODE      0x13  // Start measurement at 4lx resolution. Measurement time is approx 16ms.
#define BH1750_ONE_TIME_HIGH_RES_MODE       0x20  // Start measurement at 1lx resolution. Measurement time is approx 120ms.   Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_HIGH_RES_MODE_2     0x21  // Start measurement at 0.5lx resolution. Measurement time is approx 120ms. Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_LOW_RES_MODE        0x23  // Start measurement at 1lx resolution. Measurement time is approx 120ms.   Device is automatically set to Power Down after measurement.

#define BH17_ADDR                           BH1750_I2CADDR << 1
#define BH17_CONFIG                         BH1750_ONE_TIME_HIGH_RES_MODE_2

/****************************************************************************
* BH1750 Class
*****************************************************************************/
class BH1750
{
public:
  bool bBH1750InitState;
  uint16_t uLuxCurrent;

  bool BH1750Init();
  void BH1750DeInit();
  bool GetLux();

private:
  bool PowerDownBH1750();
};
#endif /* BH1750_H*/
