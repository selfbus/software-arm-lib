/**************************************************************************//**
 * @file    app_main.cpp
 * @brief   A simple application which will read Lux and RTC from
 *          LPC1115 Dev Board using I2C class with a timer and the timer interrupt.
 *
 *          needs BCU1 version of the sblib library
 *          needs at least a 64KB LPC111x.
 *
 *          for DBG_PRINT_LUX, DBG_PRINT_RTC or DBG_PRINT_DHT
 *          "Enable printf float" in Prj settings -> Managed Linker Script
 *
 *
 * @author Erkan Colak <erkanc@gmx.de> Copyright (c) 2015
 * @author Mario Theodoridis Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/core.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/types.h>
#include <sblib/io_pin_names.h>
#include <sblib/i2c.h>

#define DBG_LUX             1 ///< BH1750 Lux
#define DBG_PRINT_LUX       1

#define DBG_RTC             1 ///< Ds3231 RTC
#define DBG_PRINT_RTC       1
#define DBG_PRINT_RTC_ALARM 1

#define SET_RTC_INITIAL_TIME  0  ///< Change this from "1" to "0" after the time was set successfully
#define SET_RTC_ALARM1_ALARM2 0  ///< Change this from "1" to "0" after the ALARM1|2 was set successfully

#define DBG_DHT             1 ///< DHT22
#define DBG_PRINT_DHT       1

#if DBG_PRINT_LUX or DBG_PRINT_RTC or DBG_PRINT_DHT
# include <stdio.h>          // "Enable printf float" in Prj settings -> Managed Linker Script
#endif


#if DBG_LUX
#   include <sblib/i2c/bh1750.h>
#endif
#if DBG_RTC
#   include <sblib/i2c/ds3231.h>
#endif
#if DBG_DHT
#   include <sblib/sensors/dht.h>
#endif

#if DBG_LUX
  BH1750 bh;                 // BH1750
#endif
#if DBG_RTC
  Ds3231 rtc;                // Ds3231
#endif

#if DBG_DHT
  DHT dht;                   // DHT 1st
#endif

#define READ_TIMER 1000      ///> Read values timer in Milliseconds

bool bReadTimer= false;      ///> Condition to read values if timer reached

/**
 * Handler for the timer interrupt.
 */
extern "C" void TIMER32_0_IRQHandler()
{
    // Clear the timer interrupt flags, otherwise the interrupt handler is called again immediately after returning.
    timer32_0.resetFlags();
    bReadTimer= true;
}

#if SET_RTC_INITIAL_TIME
/**
 * Set the RTC Time and Calender. Set only one time!
 */
bool WriteInitTime()
{
  bool bRet= false;
  // default, use bit masks in ds3231.h for desired operation
  ds3231_cntl_stat_t rtc_control_status = {0,0};
  rtc.SetCtrlStatReg(rtc_control_status);

  ds3231_time_t rtc_time;
  rtc_time.mode       =  0;     // Set 0 for 24hr (Set 1 for 12hr.)
  rtc_time.hours      = 22;     // The hour (0-23)
  rtc_time.minutes    = 15;     // The minute (0-59)
  rtc_time.seconds    = 30;     // The second (0-59)
  bRet= rtc.SetTime(rtc_time);  // Set the above time (hh:mm:ss)--> "22:15:30"

  ds3231_calendar_t rtc_calendar;
  rtc_calendar.day    = 7 ;     // day of week, 1 for Sunday (1-7)  /1:Sunday 2:Mon 3:Tue 4:Wen 5:Thu 6:Fr 7:Sat
  rtc_calendar.date   = 07;     // day of month (1-31)
  rtc_calendar.month  = 11;     // month, 1 for January (1-12)
  rtc_calendar.year   = 15;     // year (0-99)
  bRet= rtc.SetCalendar(rtc_calendar);  //Set the above calendar  --> "Saturday 07 November 2015"

  return bRet;
}
#endif

#if SET_RTC_ALARM1_ALARM2
/**
 * Set the RTC Alarm1 and Alarm 2. Set only one time!
 */
bool SetRTCAlarm()
{
  bool bRet= false;

  //+++++++++++++ ALARM 1 +++++++++++++++
  ds3231_alrm_t alarm1;
  alarm1.am1    = false; // Match every defined second. Here: alarm.seconds= 15!
  alarm1.am2    = true;  // | For more details check "ds3231_alrm_t" in ds3231.h!
  alarm1.am3    = true;  // | For more details check "ds3231_alrm_t" in ds3231.h!
  alarm1.am4    = true;  // | For more details check "ds3231_alrm_t" in ds3231.h!

  alarm1.dy_dt  = false; // True for Day / False for Date
  alarm1.date   = 31;    // day of month (1-31)
  alarm1.day    = 7;     // day of week, 1 for Sunday (1-7)  /1:Sunday 2:Mon 3:Tue 4:Wen 5:Thu 6:Fr 7:Sat

  alarm1.mode   = false; // False: AM / True: PM
  alarm1.hours  = 12;    // The hour (0-23)
  alarm1.minutes= 13;    // The minute (0-59)
  alarm1.seconds= 15;    // The second (0-59)

  if( rtc.SetAlarm(alarm1, ALARM_1)) { // Set Alarm1 to --> Alarm 1 goes ON: Every Day, every Hour, on every Minute, on exact Second = "15"!
      bRet= rtc.TurnOnAlarm(ALARM_1);        // Now turn On the Alarm2
  }

  //+++++++++++++ ALARM 2 +++++++++++++++
  ds3231_alrm_t alarm2;
  alarm2.am2    = true;  // Once per minute (only at seconds = 00)
  alarm2.am3    = true;  // | For more details check "ds3231_alrm_t" in ds3231.h!
  alarm2.am4    = true;  // | For more details check "ds3231_alrm_t" in ds3231.h!

  alarm2.dy_dt  = false; // True for Day / False for Date
  alarm2.date   = 31;    // day of month (1-31)
  alarm2.day    = 7;     // day of week, 1 for Sunday (1-7)  /1:Sunday 2:Mon 3:Tue 4:Wen 5:Thu 6:Fr 7:Sat

  alarm2.mode   = false; // False: AM / True: PM
  alarm2.hours  = 14;    // The hour (0-23)
  alarm2.minutes= 20;    // The minute (0-59)

  if( rtc.SetAlarm(alarm2, ALARM_2)) { // Set Alarm2 to --> Alarm 2 goes ON: Every Day, every Hour, on every Minute only at Second = "00"!
      bRet= rtc.TurnOnAlarm(ALARM_2);        // Now turn On the Alarm2!
  }
  return bRet;
}
#endif

/**
 * Initialize the application.
 */
void setup()
{

#if DBG_PRINT_LUX or DBG_PRINT_RTC or DBG_PRINT_DHT
    printf("Example-i2c application started\n");
#endif

#if DBG_RTC
    rtc.Ds3231Init();           // Initialize Ds3231
    // WriteInitTime();         // Comment in, if you want setup the RTC TIME/Calendar
    // SetRTCAlarm();           // Comment in, if you want to set the Alarm1|2
#endif

#if DBG_LUX
  bh.begin();            // Initialize BH1750
#endif

#if DBG_DHT
    dht.DHTInit(PIO2_2, DHT22); // Use the DHT22 sensor on PIN
#endif

/*
    if(I2C::Instance()->bI2CIsInitialized) {  // I2CScan
        I2C::Instance()->I2CScan();           // check .I2CScan_State ans .I2CScan_uAdress
    }
*/

    // LED Initialize
    pinMode(PIN_INFO, OUTPUT);	 // Info LED (yellow)
    pinMode(PIN_RUN, OUTPUT);	 // Run  LED (green)
    pinMode(PIN_PROG, OUTPUT);   // Prog LED (red)

    // LED Set Initial Value (ON|OFF)
    digitalWrite(PIN_RUN, 0);    // Info LED (yellow), will be toggled with dht function (blink on read success)
    digitalWrite(PIN_INFO, 0);   // Run  LED (green), will be toggled with LUX function (off if Lux == 0)
    digitalWrite(PIN_PROG, 1);   // Prog LED (red), will be toggled with rtc function

    enableInterrupt(TIMER_32_0_IRQn);                   // Enable the timer interrupt
    timer32_0.begin();                                  // Begin using the timer
    timer32_0.prescaler((SystemCoreClock / 1000) - 1);  // Let the timer count milliseconds
    timer32_0.matchMode(MAT1, RESET | INTERRUPT);       // On match of MAT1, generate an interrupt and reset the timer
    timer32_0.match(MAT1, READ_TIMER);                  // Match MAT1 when the timer reaches this value (in milliseconds)
    timer32_0.start();                                  // Start now the timer
}

#if DBG_LUX
/**
 * Read LUX
 */
void ReadLux() {
  // Read I2C LUX From BH1750!
  if (bh.measurementReady(true)) {
    bReadTimer = false;   // Reset Read Timer
    float light = bh.readLightLevel();
    // Switch off the info LED if light is low else on
    digitalWrite(PIN_INFO, (light < 50));
#if DBG_PRINT_LUX
    printf("Lux: %d\n", (int)light);
#endif
  }
}
#endif

#if DBG_DHT
/**
 * Read the DHT Temperature and Humidity
 */
bool ReadTempHum()
{
  bool bRet= dht.readData();
  if(bRet)
  {
    digitalWrite(PIN_RUN, !digitalRead(PIN_RUN));
#if DBG_PRINT_DHT
    printf("     Temperature: %4.2f C \n", dht._lastTemperature );
    printf("        Humidity: %4.2f\n",dht._lastHumidity);
    printf("Dew point (fast): %4.2f\n",dht.CalcdewPointFast(dht._lastTemperature, dht._lastHumidity));
/*
    printf("       Dew point: %4.2f (FastCalc: %4.2f)\r\n",
            dht.CalcdewPoint(dht._lastTemperature, dht._lastHumidity),
            dht.CalcdewPointFast(dht._lastTemperature, dht._lastHumidity));
*/
    bRet= true;
  } else printf("Err %i \r\n",dht._lastError);
#else
  }
#endif

  return bRet;
}
#endif

#if DBG_RTC
/**
 * Read the RTC Time, Calendar, Alarm1, Alarm2 and the RTC Temperature
 */
void ReadTimeDate()
{
   ds3231_time_t rtc_time;         rtc.GetTime(&rtc_time);
   ds3231_calendar_t rtc_calendar; rtc.GetCalendar(&rtc_calendar);

#if DBG_PRINT_RTC
   printf("Zeit: %02d.%02d.%02d - %02d:%02d:%02d\n",rtc_calendar.date,rtc_calendar.month, rtc_calendar.year,rtc_time.hours, rtc_time.minutes, rtc_time.seconds);
   printf("Temp: %4.2f C\n",rtc.GetTemperature());
#endif

   ds3231_alrm_t alarm;
   if( rtc.GetAlarm(&alarm, ALARM_1) )  // Get Alarm
   {
     if(rtc.CheckAlarm(ALARM_1))
     {
#if DBG_PRINT_RTC
       printf("++++ Alarm 1 ++++");printf("\n");
#endif
       digitalWrite(PIN_PROG, !digitalRead(PIN_PROG));
       rtc.ResetAlarm(ALARM_1);
     }
#if DBG_PRINT_RTC_ALARM
     else printf("Timer Alarm 1: %d:%d:%d\n",alarm.hours, alarm.minutes, alarm.seconds);
#endif
   }

   if( rtc.GetAlarm(&alarm, ALARM_2) )
   {
     if(rtc.CheckAlarm(ALARM_2))
     {
#if DBG_PRINT_RTC
       printf("++++ Alarm 2 ++++");printf("\n");
#endif
       digitalWrite(PIN_PROG, !digitalRead(PIN_PROG));
       rtc.ResetAlarm(ALARM_2);
     }
#if DBG_PRINT_RTC_ALARM
     else printf("Timer Alarm 2: %d:%d\n",alarm.hours, alarm.minutes);
#endif
   }
}
#endif

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    if(bReadTimer)
    {
#if DBG_LUX
        ReadLux();
#endif
#if DBG_RTC
        ReadTimeDate();
#endif
#if DBG_DHT
        ReadTempHum();
#endif
        bReadTimer=false;
    }
    // Sleep until the next interrupt happens
    __WFI();
}

/**
 * The main processing loop.
 */
void loop()
{
    // will never be called in this example
}
