/*
 *  ds3231.cpp - DS3241 - Inter-Integrated Circuit (I²C).
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

#include <sblib/i2c/ds3231.h>

/*****************************************************************************
** Function name:  Ds3231Init
**
** Descriptions:   Initialize the Ds3231
**
** parameters:     none
**
** Returned value: true on success, false on failure
**
*****************************************************************************/
void Ds3231::Ds3231Init()
{
  i2c_lpcopen_init();
}

/*****************************************************************************
** Function name:  SetTime
**
** Descriptions:   Sets the time on DS3231 Struct data is in integrer format,
**                 not BCD. Fx will convert
**
** parameters:     time - struct cotaining time data;
**
** Returned value: TRUE on success, FALSE on failure
**
** Example: //time = 12:00:00 AM 12hr mode
**           ds3231_time_t time = {12, 0, 0, 0, 1}
**           uint16_t rtn_val;
**           rtn_val = rtc.SetTime(time);
**
*****************************************************************************/
bool Ds3231::SetTime(ds3231_time_t time)
{
  uint8_t data[] = {0,0,0,0};
  uint8_t data_length = 0;
  uint8_t max_hour = 24;

  data[data_length++] = SECONDS;
  data[data_length++] = uchar_2_bcd(time.seconds);
  data[data_length++] = uchar_2_bcd(time.minutes);

  //format Hours register
  data[data_length] = uchar_2_bcd(time.hours);
  if(time.mode)
  {
    max_hour = max_hour/2;

    data[data_length] |= MODE;
    if(time.am_pm)
    {
      data[data_length] |= AM_PM;
    }
  }
  else
  {
    max_hour = max_hour - 1;
  }
  data_length++;

  //Make sure data is within range.
  if(! ((time.seconds > 59) || (time.minutes > 59) || (time.hours > max_hour)) ) {
    return Chip_I2C_MasterSend(I2C0, DS3231_I2C_ADRS,
                               (const uint8_t*) data, data_length) == data_length;
  }
  return false;
}

/*****************************************************************************
** Function name:  GetTime
**
** Descriptions:   Gets the time on DS3231
**
** parameters:     [in]  time - pointer to struct for storing time data
**                 [out] time - contains current integrer rtc time
**
** Returned value: TRUE on success, FALSE on failure
**
** Example: //time = 12:00:00 AM 12hr mode
**           ds3231_time_t time = {12, 0, 0, 0, 1}
**           uint16_t rtn_val;
**           rtn_val = rtc.GetTime(&time);
**
*****************************************************************************/
bool Ds3231::GetTime(ds3231_time_t* time)
{
  uint8_t data[3];

  data[0] = SECONDS;
  if(Chip_I2C_MasterSend(I2C0, DS3231_I2C_ADRS, (const uint8_t*) data, 1) == 1 &&
     Chip_I2C_MasterRead(I2C0, DS3231_I2C_ADRS, data, 3) == 3)
  {
    time->seconds = bcd_2_uchar(data[0]);
    time->minutes = bcd_2_uchar(data[1]);
    time->am_pm = (data[2]&AM_PM);
    time->mode = (data[2]&MODE);

    if(time->mode) time->hours = bcd_2_uchar((data[2]&0x1F));
    else time->hours = bcd_2_uchar((data[2]&0x3F));

    return true;
  } else return false;
}

/*****************************************************************************
** Function name:  SetCalendar
**
** Descriptions:   Sets the calendar on DS3231
**
** parameters:     [in] calendar - struct cotaining calendar data
**
** Returned value: TRUE on success, FALSE on failure
**
** Example:  //see datasheet for calendar format
**            ds3231_calendar_t calendar = {1, 1, 1, 0};
**            uint16_t rtn_val;
**            rtn_val = rtc.SetCalendar(calendar);
**
*****************************************************************************/
bool Ds3231::SetCalendar(ds3231_calendar_t calendar)
{
  uint8_t data[] = {0,0,0,0,0};
  uint8_t data_length = 0;

  data[data_length++] = DAY;
  data[data_length++] = uchar_2_bcd(calendar.day);
  data[data_length++] = uchar_2_bcd(calendar.date);
  data[data_length++] = uchar_2_bcd(calendar.month);
  data[data_length++] = uchar_2_bcd(calendar.year);

  //Make sure data is within range.
  if(! (((calendar.day < 1) || (calendar.day > 7)) || ((calendar.date < 1) ||
    (calendar.date > 31)) || ((calendar.month < 1) || (calendar.month > 12)) ||
    (calendar.year > 99) ) )
  {
    return Chip_I2C_MasterSend(I2C0, DS3231_I2C_ADRS,
                               (const uint8_t*) data, data_length) == data_length;
  }
  return false;
}

/*****************************************************************************
** Function name:  GetCalendar
**
** Descriptions:   Gets the calendar on DS3231
**
** parameters:     [in]  calendar - pointer to struct for storing calendar data
**                 [out] calendar - contains current integer rtc calendar data
**
** Returned value: TRUE on success, FALSE on failure
**
** Example: //see datasheet for calendar format
**           ds3231_calendar_t calendar = {1, 1, 1, 0};
**           uint16_t rtn_val;
**           rtn_val = rtc.GetCalendar(&calendar);
**
*****************************************************************************/
bool Ds3231::GetCalendar(ds3231_calendar_t* calendar)
{
  uint8_t data[4];

  data[0] = DAY;
  if( Chip_I2C_MasterSend(I2C0, DS3231_I2C_ADRS, (const uint8_t*) data, 1) == 1 &&
      Chip_I2C_MasterRead(I2C0, DS3231_I2C_ADRS, data, 4) == 4)
  {
    calendar->day = bcd_2_uchar(data[0]);
    calendar->date = bcd_2_uchar(data[1]);
    calendar->month = bcd_2_uchar((data[2]&0x7F));
    calendar->year = bcd_2_uchar(data[3]);
    //Century = data[1] & 0x80;

    return true;
  } else return false;
}

/*****************************************************************************
** Function name:  SetAlarm
**
** Descriptions:   Set either Alarm1 or Alarm2 of DS3231
**
** parameters:     [in] alarm - struct cotaining alarm data
**                 [in] one_r_two - TRUE for Alarm1 and FALSE for Alarm2
**
** Returned value: TRUE on success, FALSE on failure
**
** Example:  //see ds3231.h for ds3231_alrm_t members and alarm format
**           ds3231_alrm_t alarm;
**           uint16_t rtn_val;
**           rtn_val = rtc.SetAlarm(alarm, FALSE);
**
*****************************************************************************/
bool Ds3231::SetAlarm(ds3231_alrm_t alarm, bool bAlarmNumber)
{
  uint8_t data[] = {0,0,0,0,0};
  uint8_t data_length = 0;
  uint8_t max_hour = 24;
  uint8_t mask_var = 0;

  //setting alarm 1 or 2?
  if(bAlarmNumber)
  {
    data[data_length++] = ALRM1_SECONDS;

    //config seconds register
    if(alarm.am1)
    {
       mask_var |= ALRM_MASK;
    }
    data[data_length++] =  (mask_var | uchar_2_bcd(alarm.seconds));
    mask_var = 0;

    //config minutes register
    if(alarm.am2)
    {
       mask_var |= ALRM_MASK;
    }
    data[data_length++] =  (mask_var | uchar_2_bcd(alarm.minutes));
    mask_var = 0;

    //config hours register
    if(alarm.am3)
    {
       mask_var |= ALRM_MASK;
    }
    if(alarm.mode)
    {
       max_hour = max_hour/2;
       mask_var |= MODE;
       if(alarm.am_pm)
       {
          mask_var |= AM_PM;
       }
    }
    else
    {
       max_hour = max_hour - 1;
    }
    data[data_length++] =  (mask_var | uchar_2_bcd(alarm.hours));
    mask_var = 0;

    if(alarm.am4)
    {
       mask_var |= ALRM_MASK;
    }

    //config day/date register
    if(alarm.dy_dt)
    {
       mask_var |= DY_DT;
       data[data_length++] =  (mask_var | uchar_2_bcd(alarm.day));
    }
    else
    {
       data[data_length++] =  (mask_var | uchar_2_bcd(alarm.date));
    }
    mask_var = 0;
  }
  else
  {
    data[data_length++] = ALRM2_MINUTES;

    //config minutes register
    if(alarm.am2)
    {
      mask_var |= ALRM_MASK;
    }
    data[data_length++] =  (mask_var | uchar_2_bcd(alarm.minutes));
    mask_var = 0;

    //config hours register
    if(alarm.am3)
    {
      mask_var |= ALRM_MASK;
    }
    if(alarm.mode)
    {
      max_hour = max_hour/2;
      mask_var |= MODE;
      if(alarm.am_pm)
      {
        mask_var |= AM_PM;
      }
    }
    else
    {
      max_hour = max_hour - 1;
    }
    data[data_length++] =  (mask_var | uchar_2_bcd(alarm.hours));
    mask_var = 0;

    if(alarm.am4)
    {
      mask_var |= ALRM_MASK;
    }

    //config day/date register
    if(alarm.dy_dt)
    {
      mask_var |= DY_DT;
      data[data_length++] =  (mask_var | uchar_2_bcd(alarm.day));
    }
    else
    {
      data[data_length++] =  (mask_var | uchar_2_bcd(alarm.date));
    }
    mask_var = 0;
  }

  //Make sure data is within range.
  if( !( (bAlarmNumber && alarm.seconds > 59) || (alarm.minutes > 59) ||
      (alarm.hours > max_hour) || (alarm.day < 1) || (alarm.day > 7) ||
      (alarm.date < 1) || (alarm.date > 31) ))
  {
    return Chip_I2C_MasterSend(I2C0, DS3231_I2C_ADRS,
                               (const uint8_t*) data, data_length) == data_length;
  } else return false;
}

/*****************************************************************************
** Function name:  GetAlarm
**
** Descriptions:   Get either Alarm1 or Alarm2 of DS3231
**
** parameters:     [in] alarm - pointer to struct for storing alarm data;
**                 [in] one_r_two - TRUE for Alarm1 and FALSE for Alarm2
**                 [out] alarm - contains integer alarm data
**
** Returned value: TRUE on success, FALSE on failure
**
** Example: //see ds3231.h for .members and datasheet for alarm format
**          ds3231_alrm_t alarm;
**          uint16_t rtn_val;
**          rtn_val = rtc.GetAlarm(&alarm, FALSE);
**
*****************************************************************************/
bool Ds3231::GetAlarm(ds3231_alrm_t* alarm, bool bAlarmNumber)
{
  bool bRet= false;
  uint8_t data[4];

  if(bAlarmNumber)
  {
    data[0] = ALRM1_SECONDS;
    if( Chip_I2C_MasterSend(I2C0, DS3231_I2C_ADRS, (const uint8_t*) data, 1) == 1 &&
        Chip_I2C_MasterRead(I2C0, DS3231_I2C_ADRS, data, 4) == 4)
    {
      bRet = true;
      alarm->seconds = bcd_2_uchar(data[0]&0x7F);
      alarm->am1 = (data[0]&ALRM_MASK);
      alarm->minutes = bcd_2_uchar(data[1]&0x7F);
      alarm->am2 = (data[1]&ALRM_MASK);
      alarm->am3 = (data[2]&ALRM_MASK);
      alarm->am_pm = (data[2]&AM_PM);
      alarm->mode = (data[2]&MODE);

      if(alarm->mode) alarm->hours = bcd_2_uchar((data[2]&0x1F));
      else alarm->hours = bcd_2_uchar((data[2]&0x3F));

      if(data[3] & DY_DT)
      {
        alarm->dy_dt = 1;
        alarm->day = bcd_2_uchar(data[3]&0x0F);
      } else alarm->date = bcd_2_uchar(data[3]&0x3F);

      alarm->am4 = (data[3]&ALRM_MASK);
    }
  }
  else
  {
    data[0] = ALRM2_MINUTES;

    if( Chip_I2C_MasterSend(I2C0, DS3231_I2C_ADRS, (const uint8_t*) data, 1) == 1 &&
        Chip_I2C_MasterRead(I2C0, DS3231_I2C_ADRS, data, 3) == 3)
    {
      bRet = true;
      alarm->minutes = bcd_2_uchar(data[0]&0x7F);
      alarm->am2 = (data[0]&ALRM_MASK);

      alarm->am3 = (data[1]&ALRM_MASK);
      alarm->am_pm = (data[1]&AM_PM);
      alarm->mode = (data[1]&MODE);

      if(alarm->mode) alarm->hours = bcd_2_uchar((data[1]&0x1F)); // 12-hour
      else alarm->hours = bcd_2_uchar((data[1]&0x3F)); // 24-hour

      if(data[2] & DY_DT)
      {
        alarm->dy_dt = 1;
        alarm->day = bcd_2_uchar(data[2]&0x0F); // alarm is by day of week
      } else alarm->date = bcd_2_uchar(data[2]&0x3F); // alarm is by date

      alarm->am4 = (data[2]&ALRM_MASK);
    }
  }
  return bRet;
}

/*****************************************************************************
** Function name:  SetCtrlStatReg
**
** Descriptions:   Set control and status registers of DS3231
**
** parameters:     [in] data - Struct containing control and status register data
**                 [in] Set true to set: bSetControl, bSetStatus (default both true)
**
** Returned value: TRUE on success, FALSE on failure
**
** Example: //do not use 0xAA, see datasheet for appropriate data
**           ds3231_cntl_stat_t data = {0xAA, 0xAA};
**           rtn_val = rtc.SetCtrlStatReg(data);
**
*****************************************************************************/
bool Ds3231::SetCtrlStatReg(ds3231_cntl_stat_t data, bool bSetControl, bool bSetStatus)
{
  uint8_t local_data[] = {0,0,0};
  uint8_t data_length=0;
  local_data[data_length++] = CONTROL;
  if( bSetControl) local_data[data_length++] = data.control;
  if( bSetStatus ) local_data[data_length++] = data.status;

  // users responsibility to make sure data is logical
  return Chip_I2C_MasterSend(I2C0, DS3231_I2C_ADRS,
                             (const uint8_t*) local_data, 3) == 3;
}

/*****************************************************************************
** Function name:  GetCtrlStatReg
**
** Descriptions:   Get control and status registers of DS3231
**
** parameters:     [in] data - pointer to struct for storing
**                             control and status register data
**                 [out] data - contains control and status registers data
**
** Returned value: TRUE on success, FALSE on failure
**
** Example: //do not use 0xAA, see datasheet for appropriate data
**          ds3231_cntl_stat_t data = {0xAA, 0xAA};
**          rtn_val = rtc.GetCtrlStatReg(&data);
**
*****************************************************************************/
bool Ds3231::GetCtrlStatReg(ds3231_cntl_stat_t* data)
{
  uint8_t local_data[] = {0,0,0};
  local_data[0] = CONTROL;
  local_data[1] = data->control;
  local_data[2] = data->status;

  if( Chip_I2C_MasterSend(I2C0, DS3231_I2C_ADRS, (const uint8_t*) local_data, 1) == 1 &&
      Chip_I2C_MasterRead(I2C0, DS3231_I2C_ADRS, local_data, 2) == 2)
  {
    data->control = local_data[0];
    data->status = local_data[1];
    return true;

  } else return false;
}

/*****************************************************************************
** Function name:  GetEpoch
**
** Descriptions:   Get epoch time based on current RTC time and date. DS3231
**                 must be configured and running before this fx is called
**
** parameters:     none
**
** Returned value: epoch time
**
** Example: time_t epoch_time;
**          epoch_time = rtc.GetEpoch();
**
*****************************************************************************/
time_t Ds3231::GetEpoch(void)
{
  struct tm sys_time;                           //system vars
  ds3231_time_t         rtc_time = {0,0,0,0,0}; //RTC vars
  ds3231_calendar_t rtc_calendar = {0,0,0,0};

  if( this->GetCalendar(&rtc_calendar) &&  this->GetTime(&rtc_time) )
  {
    sys_time.tm_wday = rtc_calendar.day - 1;
    sys_time.tm_mday = rtc_calendar.date;
    sys_time.tm_mon = rtc_calendar.month - 1;
    sys_time.tm_year = rtc_calendar.year + 100;

    if(rtc_time.mode)     //check for 12hr or 24hr mode
    {
      if(rtc_time.am_pm  && (rtc_time.hours != 12)) sys_time.tm_hour = rtc_time.hours + 12; //check am/pm
      else sys_time.tm_hour = rtc_time.hours;
    } else sys_time.tm_hour = rtc_time.hours; //24hr mode

    sys_time.tm_min = rtc_time.minutes;
    sys_time.tm_sec = rtc_time.seconds;
  }

  return(mktime(&sys_time)); //make epoch time
}

/*****************************************************************************
** Function name:  CheckAlarm
**
** Descriptions:   Return true, if configured alarm is activated.
**
** parameters:     ALARM_1 or ALARM_2
**
** Returned value: TRUE on success, FALSE on failure
**
*****************************************************************************/
bool Ds3231::CheckAlarm(bool bAlarmNumber)
{
  bool bRet = 0;
  ds3231_cntl_stat_t data;
  if (bAlarmNumber) {
    data.control = A1IE;
    data.status = A1F;
  } else {
    data.control = A2IE;
    data.status = A2F;
  }
  if (this->GetCtrlStatReg(&data)) {
    if (bAlarmNumber) {
      bRet = /*(data.control & 0x01) && */(data.status & 0x01); // ALARM_1
    } else {
      bRet = /*(data.control & 0x02) &&*/ (data.status & 0x02); // ALARM_2
    }
  }
  return bRet;
}

/*****************************************************************************
** Function name:  ResetAlarm
**
** Descriptions:   Will reset the alarm! (TurnOff)
**
** parameters:     ALARM_1 or ALARM_2
**
** Returned value: TRUE on success, FALSE on failure
**
*****************************************************************************/
bool Ds3231::ResetAlarm(bool bAlarmNumber)
{
  bool bRet = 0;
  ds3231_cntl_stat_t data;
  if (bAlarmNumber) {
    data.control = A1IE;
    data.status = A1F;
  } else {
    data.control = A2IE;
    data.status = A2F;
  }
  if (this->GetCtrlStatReg(&data)) {
    data.control = data.control & (bAlarmNumber ? 0xFE : 0xFD);
    data.status = data.status & (bAlarmNumber ? 0xFE : 0xFD);
    bRet = this->SetCtrlStatReg(data);
  }
  return bRet;
}

/*****************************************************************************
** Function name:  TurnOnAlarm
**
** Descriptions:   Turns on alarm number "ALARM_1 or "ALARM_2".
**
** parameters:     ALARM_1 or ALARM_2
**
** Returned value: TRUE on success, FALSE on failure
**
*****************************************************************************/
bool Ds3231::TurnOnAlarm(bool bAlarmNumber)
{
  bool bRet = 0;
  ds3231_cntl_stat_t data;
  if (bAlarmNumber) {
    data.control = A1IE;
    data.status = A1F;
  } else {
    data.control = A2IE;
    data.status = A2F;
  }
  if (this->GetCtrlStatReg(&data)) {
    data.control = data.control & (bAlarmNumber ? 0x05 : 0x06);
    data.status = data.status & (bAlarmNumber ? 0x05 : 0x06);
    bRet = this->SetCtrlStatReg(data);
  }
  return bRet;
}

/*****************************************************************************
** Function name: EnableOscillator
**
** Descriptions:  Turns oscillator on or off. True is on, false is off.
**
** parameters:   If bBattery is true, turns on even for battery-only
**               operation, otherwise turns off if Vcc is off.
**               RS_bit_SQW: must be QUARE-WAVE OUTPUT FREQUENCY (ds3231.h)
**
** Returned value: TRUE on success, FALSE on failure
**
*****************************************************************************/
bool Ds3231::EnableOscillator(bool TF, bool bBattery, uint8_t RS_bit_SQW)
{
  bool bRet=false;
  ds3231_cntl_stat_t data;

  if( this->GetCtrlStatReg(&data) )
  {
    // read control byte, but zero out current state of RS2 and RS1.
    data.control= data.control & 0xE7;
    if (bBattery) {
      // turn on BBSQW flag
      data.control = data.control | 0x40;
    } else {
      // turn off BBSQW flag
      data.control = data.control & 0xBF;
    }

    if (TF) {
      // set ~EOSC to 0 and INTCN to zero.
      data.control = data.control & 0x7B;
    } else {
      // set ~EOSC to 1, leave INTCN as is.
      data.control = data.control | 0x80;
    }

    data.control = data.control | RS_bit_SQW;
    // Write the control bits
    bRet= this->SetCtrlStatReg(data, true, false);
  }
  return bRet;
}

/*****************************************************************************
** Function name:  GetTemperature
**
** Descriptions:   Get temperature data of DS3231
**
** parameters:     [out] uTemp for RAW and fTemp for float
**
** Returned value: TRUE on success, FALSE on failure
**
*****************************************************************************/
float Ds3231::GetTemperature()
{
  float fRet=-999;
  uint8_t data[2];
  data[0] = MSB_TEMP;
  if( Chip_I2C_MasterSend(I2C0, DS3231_I2C_ADRS, (const uint8_t*) data, 1) == 1 &&
      Chip_I2C_MasterRead(I2C0, DS3231_I2C_ADRS, data, 4) == 4)
  {
    //iRet= int(data[0] << 8); iRet |= int(data[1]);  //HEX
    //iRet= int(data[0]+data[1]/256.0);                 //approx.
    fRet= (float)((data[0]) + 0.25*(data[1]>>6));
  }
  return fRet;
}

/*****************************************************************************
** Function name:  uchar_2_bcd
**
** Descriptions:   Private mmber fx, converts unsigned char to BCD
**
** parameters:     [in] data - 0-255
**
** Returned value: bcd_result = BCD representation of data
**
** Example: noene
**
*****************************************************************************/
uint16_t Ds3231::uchar_2_bcd(uint8_t data)
{
  uint16_t bcd_result = 0;
   
  //Get hundreds
  bcd_result |= ((data/100) << 8);
  data = (data - (data/100)*100);
   
  //Get tens
  bcd_result |= ((data/10) << 4);
  data = (data - (data/10)*10);
   
  //Get ones
  bcd_result |= data;
   
  return(bcd_result);
}

/*****************************************************************************
** Function name:  bcd_2_uchar
**
** Descriptions:   Private mmber fx, converts BCD to a uint8_t
**
** parameters:     [in] bcd - 0-99
**
** Returned value: rtn_val = integer rep. of BCD
**
** Example: noene
**
*****************************************************************************/
uint8_t Ds3231::bcd_2_uchar(uint8_t bcd)
{
  uint8_t rtn_val = 0;

  rtn_val += ((bcd&0xf0)>>4)*10;
  rtn_val += (bcd&0x000f);

  return rtn_val;
}
