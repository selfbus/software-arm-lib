/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_I2C_DS3231 i2c DS3231 real time clock (RTC) example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Read real time clock (RTC) using I2C class with a timer and the timer interrupt
 *
 * @note    Debug build needs at least a 64KB LPC111x.
 *
 * @{
 *
 * @author Erkan Colak <erkanc@gmx.de> Copyright (c) 2015
 * @author Mario Theodoridis Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @author Doumanix <doumanix@gmx.de> Copyright (c) 2023
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#include <sblib/types.h>
#include <sblib/io_pin_names.h>
#include <sblib/i2c.h>
#include <sblib/eibBCU1.h>
#include <sblib/serial.h>
#include <sblib/i2c/ds3231.h>

#define DBG_PRINT_RTC       1
#define DBG_PRINT_RTC_ALARM 1

#define SET_RTC_INITIAL_TIME  0  ///< Change this from "1" to "0" after the time was set successfully
#define SET_RTC_ALARM1_ALARM2 0  ///< Change this from "1" to "0" after the ALARM1|2 was set successfully

#define READ_TIMER (2000) ///> Read values timer in Milliseconds
bool bReadTimer = false;  ///> Condition to read values if timer reached

Ds3231 rtc;                // Ds3231
BCU1 bcu = BCU1();

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
BcuBase* setup()
{
    serial.setRxPin(PIO1_6); // @ swd/jtag connector
    serial.setTxPin(PIO1_7); // @ swd/jtag connector

    // serial.setRxPin(PIO2_7); // @ 4TE-ARM Controller pin 1 on connector SV3 (ID_SEL)
    // serial.setTxPin(PIO2_8); // @ 4TE-ARM Controller pin 2 on connector SV3 (ID_SEL)

    serial.begin(115200);
    serial.println("Selfbus I2C DS3231 real time clock (RTC) example");

    rtc.Ds3231Init();           // Initialize Ds3231
    // WriteInitTime();         // Comment in, if you want setup the RTC TIME/Calendar
    // SetRTCAlarm();           // Comment in, if you want to set the Alarm1|2

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

    return (&bcu);
}

/**
 * Read the RTC Time, Calendar, Alarm1, Alarm2 and the RTC Temperature
 */
void ReadTimeDate()
{
   ds3231_time_t rtc_time;         rtc.GetTime(&rtc_time);
   ds3231_calendar_t rtc_calendar; rtc.GetCalendar(&rtc_calendar);

#if DBG_PRINT_RTC
   serial.print("Zeit: ", rtc_calendar.date, DEC, 2);
   serial.print(".", rtc_calendar.month, DEC, 2);
   serial.print(".", rtc_calendar.year, DEC, 2);
   serial.print(" - ", rtc_time.hours, DEC, 2);
   serial.print(":", rtc_time.minutes, DEC, 2);
   serial.println(":", rtc_time.seconds, DEC, 2);
   serial.print("Temp: ",rtc.GetTemperature());
   serial.println(" C");
#endif

   ds3231_alrm_t alarm;
   if( rtc.GetAlarm(&alarm, ALARM_1) )  // Get Alarm
   {
     if(rtc.CheckAlarm(ALARM_1))
     {
#if DBG_PRINT_RTC
       serial.println("++++ Alarm 1 ++++");
#endif
       digitalWrite(PIN_PROG, !digitalRead(PIN_PROG));
       rtc.ResetAlarm(ALARM_1);
     }
#if DBG_PRINT_RTC_ALARM
     else
     {
         serial.print("Timer Alarm 1: ", alarm.hours, DEC, 2);
         serial.print(":", alarm.minutes, DEC, 2);
         serial.print(":", alarm.seconds, DEC, 2);
     }
#endif
   }

   if( rtc.GetAlarm(&alarm, ALARM_2) )
   {
     if(rtc.CheckAlarm(ALARM_2))
     {
#if DBG_PRINT_RTC
       serial.println("++++ Alarm 2 ++++");
#endif
       digitalWrite(PIN_PROG, !digitalRead(PIN_PROG));
       rtc.ResetAlarm(ALARM_2);
     }
#if DBG_PRINT_RTC_ALARM
     else
    {
       serial.print("Timer Alarm 2: ", alarm.hours, DEC, 2);
       serial.println(":", alarm.minutes, DEC, 2);
    }
#endif
   }
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    if (bReadTimer)
    {
        ReadTimeDate();
        bReadTimer = false;
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
/** @}*/
