/*
 *  ds3231.cpp - DS3241 - Inter-Integrated Circuit (I²C).
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation. Based on Justin Jordan
 *  Copyright (C) 2015 Maxim Integrated Products, Inc., All Rights Reserved.
 */
#ifndef ds3231_h
#define ds3231_h

#include <time.h>


#define DS3231_I2C_ADRS 0x68 << 1    // (DS3231_I2C_ADRS << 1)

#define AM_PM     (1 << 5) 
#define MODE      (1 << 6)
#define DY_DT     (1 << 6)
#define ALRM_MASK (1 << 7)

// Control bits (control register bit masks)
#define A1IE  (1 << 0)  // 0x01 Alarm 1 Interrupt Enable
#define A2IE  (1 << 1)  // 0x02 Alarm 2 Interrupt Enable (A2IE)
#define INTCN (1 << 2)  // 0x04 Interrupt Control (INTCN)
#define RS1   (1 << 3)  // 0x08 Rate Select (RS2 and RS1)
#define RS2   (1 << 4)  // 0x10 Rate Select (RS2 and RS1)
#define CONV  (1 << 5)  // 0x20 Convert Temperature (CONV)
#define BBSQW (1 << 6)  // 0x40 Battery-Backed Square-Wave Enable (BBSQW)
#define EOSC  (1 << 7)  // 0x80 Enable Oscillator (EOSC)

// Control/Status bits (status register bit masks)
#define A1F     (1 << 0) // 0x01 Alarm 1 Flag (A1F)
#define A2F     (1 << 1) // 0x02 Alarm 2 Flag (A2F)
#define BSY     (1 << 2) // 0x04 Busy (BSY)
#define EN32KHZ (1 << 3) // 0x08 Enable 32kHz Output (EN32kHz)
#define OSF     (1 << 7) // 0x80 Oscillator Stop Flag (OSF)

// Rate Select (RS2  and  RS1) bit masks  (SQUARE-WAVE OUTPUT FREQUENCY)
#define RS_bit_SQW_1Hz      (0 << 3) //0x01 1Hz
#define RS_bit_SQW_1024Hz   (1 << 3) //0x08 1024 kHz
#define RS_bit_SQW_4096Hz   (2 << 3) //0x16 4096 kHz
#define RS_bit_SQW_8192Hz   (3 << 3) //0x24 8192 kHz

#define ALARM_1 1
#define ALARM_2 0


/*****************************************************************************
** ds3231_time_t - Struct for containing time data.
**
** Members:
** - uint32_t seconds - Use decimal value. Member fx's convert to BCD
** - uint32_t minutes - Use decimal value. Member fx's convert to BCD
** - uint32_t hours   - Use decimal value. Member fx's convert to BCD
** - bool am_pm       - TRUE for PM, same logic as datasheet
** - bool mode        - TRUE for 12 hour, same logic as datasheet
*****************************************************************************/
typedef struct
{
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  bool am_pm;
  bool mode;
} ds3231_time_t;

/*****************************************************************************
** ds3231_calendar_t - Struct for containing calendar data.
**
** Members:
** - uint32_t day   - Use decimal value. Member fx's convert to BCD
** - uint32_t date  - Use decimal value. Member fx's convert to BCD
** - uint32_t month - Use decimal value. Member fx's convert to BCD
** - uint32_t year  - Use decimal value. Member fx's convert to BCD
*****************************************************************************/
typedef struct
{
  uint8_t day;
  uint8_t date;
  uint8_t month;
  uint8_t year;
} ds3231_calendar_t;

/*****************************************************************************
** ds3231_alrm_t - Struct for containing alarm data.
**
** Members:
** - uint32_t seconds - Use decimal value. Member fx's convert to BCD
** - uint32_t minutes - Use decimal value. Member fx's convert to BCD
** - uint32_t hours   - Use decimal value. Member fx's convert to BCD
** - uint32_t day     - Use decimal value. Member fx's convert to BCD
** - uint32_t date    - Use decimal value. Member fx's convert to BCD
**
** - bool am1        - Flag for setting alarm rate
** - bool am2        - Flag for setting alarm rate
** - bool am3        - Flag for setting alarm rate
** - bool am4        - Flag for setting alarm rate
** - bool am_pm      - TRUE for PM, same logic as datasheet
** - bool mode       - TRUE for 12 hour, same logic as datasheet
** - bool dy_dt      - TRUE for Day, same logic as datasheet
*****************************************************************************/
typedef struct
{
  /*
   * dy_dt true makes the alarm go on dy_dt = Day of Week,
   * dy_dt false makes the alarm go on dy_dt = Date of month.
   *
   * byte AlarmBits sets the behavior of the alarms:
   *  dy_dt  am1    am2    am3    am4   Rate
   *  X      1      1      1      1     Once per second
   *  X      1      1      1      0     Alarm when seconds match
   *  X      1      1      0      0     Alarm when min, sec match
   *  X      1      0      0      0     Alarm when hour, min, sec match
   *  0      0      0      0      0     Alarm when date, h, m, s match
   *  1      0      0      0      0     Alarm when DoW, h, m, s match
   *
   *  dy_dt         am2    am3    am4
   *  X             1      1      1     Once per minute (at seconds = 00)
   *  X             1      1      0     Alarm when minutes match
   *  X             1      0      0     Alarm when hours and minutes match
   *  0             0      0      0     Alarm when date, hour, min match
   *  1             0      0      0     Alarm when DoW, hour, min match
   */

  uint8_t seconds;      // Seconds is not used for alarm2
  uint8_t minutes;
  uint8_t hours;
  uint8_t day;
  uint8_t date;
                        // alarm1: "Alarm once per second"
  bool am1;             // alarm2: am1 is not used for alarm2

                        // alarm1: "Alarm when seconds match"
  bool am2;             // alarm2: "Alarm once per minute (00 seconds of every minute)"

                        // alarm1: "Alarm when minutes and seconds match"
  bool am3;             // alarm2: "Alarm when minutes match"

                        // alarm1: "Alarm when hours, minutes, and seconds match"
  bool am4;             // alarm2: "Alarm when hours and minutes match"
  bool am_pm;           // false:AM                                   true:PM ( AM/PM is only need if mode==true for 12hr.)
  bool mode;            // false: "for 24hr"                          true: "for 12hr"
  bool dy_dt;           // false: "Alarm is a date of the month."     true:"Alarm is a day of the week"
} ds3231_alrm_t;

/*****************************************************************************
** ds3231_cntl_stat_t - Struct for containing control and status register data.
**
* Members:
** - uint8_t control - Binary data for read/write of control register
** - uint8_t status  - Binary data  for read/write of status register
*****************************************************************************/
typedef struct
{
  uint8_t control;
  uint8_t status;
} ds3231_cntl_stat_t;

/****************************************************************************
* Ds3231 Class
*****************************************************************************/
class Ds3231
{
public:
  // ds3231_regs_t - enumerated DS3231 registers
  typedef enum
  {
    SECONDS,              // 0x00
    MINUTES,              // 0x01
    HOURS,                // 0x02
    DAY,                  // 0x03
    DATE,                 // 0x04
    MONTH,                // 0x05
    YEAR,                 // 0x06
    ALRM1_SECONDS,        // 0x07
    ALRM1_MINUTES,        // 0x08
    ALRM1_HOURS,          // 0x09
    ALRM1_DAY_DATE,       // 0x0a
    ALRM2_MINUTES,        // 0x0b
    ALRM2_HOURS,          // 0x0c
    ALRM2_DAY_DATE,       // 0x0d
    CONTROL,              // 0x0e - Control
    STATUS,               // 0x0f - Control/Status
    AGING_OFFSET,         // 0x10 - Aging Offset (Don't touch this register)
    MSB_TEMP,             // 0x11 - MSB of Temp
    LSB_TEMP              // 0x12 - LSB of Temp
  } ds3231_Regs_t;

  void Ds3231Init();
  bool SetTime(ds3231_time_t time);
  bool GetTime(ds3231_time_t* time);
  bool SetCalendar(ds3231_calendar_t calendar);
  bool GetCalendar(ds3231_calendar_t* calendar);
  bool SetAlarm(ds3231_alrm_t alarm, bool one_r_two);
  bool GetAlarm(ds3231_alrm_t* alarm, bool one_r_two);
  time_t GetEpoch();
  bool SetCtrlStatReg(ds3231_cntl_stat_t data,  bool bSetControl=true, bool bSetStatus=true);
  bool GetCtrlStatReg(ds3231_cntl_stat_t* data);
  bool CheckAlarm(bool one_r_two);
  bool ResetAlarm(bool one_r_two);
  bool TurnOnAlarm(bool one_r_two);
  bool EnableOscillator(bool TF, bool bBattery, uint8_t RS_bit_SQW);
  float GetTemperature();

private:
  uint16_t uchar_2_bcd(uint8_t data);
  uint8_t bcd_2_uchar(uint8_t bcd);
};
#endif /* DS3231_H */
