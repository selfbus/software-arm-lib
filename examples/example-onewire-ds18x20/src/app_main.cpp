/*
 *  A simple application which will scan and read Temperature from DS18x20
 *  family devices using the OneWire (1-Wire) Protocol implementation on
 *  a LPC1115 Dev Board. Including OneWire parasite power mode.
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */


#define DBG_PRINT       0    // Debug print to console

#if DBG_PRINT
#include <stdio.h>
#endif

#include <sblib/core.h>
#include <sblib/sensors/ds18x20.h>
#include <sblib/eib/sblib_default_objects.h>

#define USE_OWN_ROM     1      // Use you own extracted rom informations. See SetOwnDs18xDeviceRoms()
#define PORTPIN         PIO1_7 // GPIO PIN, where the 1-wire data line is connected
#define PARASITE_POWER  true   // Enable the Parasite power (check ds18x20 Datasheet )
#define READ_TIMER 500         // Read values timer in Milliseconds

bool bLastRead= false;       // Condition to read values if timer reached
bool bInitSearch= false;     // Device search state

DS18x20 ds;

#if USE_OWN_ROM
/*
 * Example how to use your own extracted rom.
 */
bool Set_Own_Ds18x_Device_Roms()
{
  // Your own extracted rom's looks like:
  // #1 - ROM 0x28,0xb4,0x14,0x5d,0x5,0x0,0x0,0xd6  Chip: DS18B20 Temp: 23.3750 Celcius
  // #2 - ROM 0x22,0x93,0xb2,0x1e,0x0,0x0,0x0,0x9c  Chip:  DS1822 Temp: 23.2500 Celcius

  ds.m_dsDev[0].addr[0]=0x28; // 0x28 Chip: DS18B20
  ds.m_dsDev[0].addr[1]=0xb4;
  ds.m_dsDev[0].addr[2]=0x14;
  ds.m_dsDev[0].addr[3]=0x5d;
  ds.m_dsDev[0].addr[4]=0x5;
  ds.m_dsDev[0].addr[5]=0x0;
  ds.m_dsDev[0].addr[6]=0x0;
  ds.m_dsDev[0].addr[7]=0xd6;

//  ds.m_dsDev[1].addr[0]=0x22; // 0x22 Chip:  DS1822
//  ds.m_dsDev[1].addr[1]=0x93;
//  ds.m_dsDev[1].addr[2]=0xb2;
//  ds.m_dsDev[1].addr[3]=0x1e;
//  ds.m_dsDev[1].addr[4]=0x0;
//  ds.m_dsDev[1].addr[5]=0x0;
//  ds.m_dsDev[1].addr[6]=0x0;
//  ds.m_dsDev[1].addr[7]=0x9c;

  ds.m_foundDevices= 1;   // Important: Set the count of your devices!
  return true;
}
#endif

/*
 * Initialize the application.
 */
void setup()
{
  ds.DS18x20Init(PORTPIN, PARASITE_POWER);    // Initialize DS18x20 (1-Wire Protocol will be Initialized automatically)
#if USE_OWN_ROM
  bInitSearch= Set_Own_Ds18x_Device_Roms();
#else
  bInitSearch= ds.Search() && ds.m_foundDevices > 0; // Scan the 1-Wire bus for DS18x devices
#endif


  // LED Initialize
  pinMode(PIO2_6, OUTPUT);   // Info LED (yellow)
  pinMode(PIO3_3, OUTPUT);   // Run  LED (green)
  pinMode(PIO2_0, OUTPUT);   // Prog LED (red)

  // LED Set Initial Value (ON|OFF)
  digitalWrite(PIO2_6, 1);   // Run  LED (yellow)         // Will be toggled with OneWire function (blinks the count of found devices )
  digitalWrite(PIO3_3, 1);   // Info LED (green)          // Will be toggled with OneWire function (blink on read success)
  digitalWrite(PIO2_0, 0);   // Prog LED (red)


  /* Doumanix debug start: first yellow LED should blink 2 times then the green LED
   * --> indicates that code and controller board work properly (at least until this point)
   * */
  digitalWrite(PIO2_6, 0); delay(400);
  digitalWrite(PIO2_6, !digitalRead(PIO2_6));delay(400);
  digitalWrite(PIO2_6, !digitalRead(PIO2_6));delay(400);
  digitalWrite(PIO2_6, !digitalRead(PIO2_6));delay(400);
  digitalWrite(PIO2_6, !digitalRead(PIO2_6));delay(1500);

  digitalWrite(PIO3_3, 0); delay(400);
  digitalWrite(PIO3_3, !digitalRead(PIO3_3));delay(400);
  digitalWrite(PIO3_3, !digitalRead(PIO3_3));delay(400);
  digitalWrite(PIO3_3, !digitalRead(PIO3_3));delay(400);
  digitalWrite(PIO3_3, !digitalRead(PIO3_3));delay(1500);
  /* Doumanix debug end*/

  enableInterrupt(TIMER_32_0_IRQn);                        // Enable the timer interrupt
  timer32_0.begin();                                       // Begin using the timer
    timer32_0.prescaler((SystemCoreClock / 1000) - 1);     // Let the timer count milliseconds
    timer32_0.matchMode(MAT1, RESET | INTERRUPT);          // On match of MAT1, generate an interrupt and reset the timer
    timer32_0.match(MAT1, READ_TIMER);                     // Match MAT1 when the timer reaches this value (in milliseconds)
  timer32_0.start();                                       // Start now the timer

}

/*
 * Read the Temperature form found DS18x Devices.
 */
bool ReadOneWireDS18xTemp()
{
  bool bRet=false;
  if(ds.readTemperatureAll())                              // Read all temperatures from .m_dsDev
  {
      digitalWrite(PIO3_3, 1);                             // Switch on the green LED to signal that we read the temperatures
      for(uint8_t j=0; j < ds.m_foundDevices; j++ )        // Iterate then devices and extract and print the temperatures.
      {
        if(ds.m_dsDev[j].lastReadOK)                       // Check if the last temperature read was OK
        {
#if DBG_PRINT
          printf("#%d - ROM ",j+1); for( uint8_t i = 0; i < 8; i++) { printf("0x%x",ds.m_dsDev[j].addr[i]); if(i<7 )printf(","); }
          printf("  Chip: %s", (ds.m_dsDev[j].addr[0] == 0x10)?"DS18S20":(ds.m_dsDev[j].addr[0] == 0x28)?"DS18B20":(ds.m_dsDev[j].addr[0] == 0x22)?" DS1822":"DS_UNKNOWN");
          printf(" Temp: %4.4f Celcius (%4.4f Farenheit - %4.4f Kelvin)\n", ds.m_dsDev[j].current_temperature,
                                                             ds.ConvertTemperature(ds.m_dsDev[j].current_temperature, FARENHEIT),
                                                             ds.ConvertTemperature(ds.m_dsDev[j].current_temperature, KELVIN));
#endif
          bRet= true;
        }
        digitalWrite(PIO2_6, 0); delay(125);               // Switch on<>off shows the count of devices which last read was OK.
        digitalWrite(PIO2_6, !digitalRead(PIO2_6));delay(125);
        digitalWrite(PIO2_6, !digitalRead(PIO2_6));
      }
      digitalWrite(PIO3_3, 0);                             // Switch off the green LED. Temperature extraction is done.
  }
#if DBG_PRINT
  if(bRet) printf("\n");
#endif
  return bRet;
}


/*
 * Handler for the timer interrupt.
 */
extern "C" void TIMER32_0_IRQHandler()
{                            // Clear the timer interrupt flags.
  timer32_0.resetFlags();    // Otherwise the interrupt handler is called again immediately after returning.
  bLastRead= true;
}

/*
 * The main processing loop.
 */
void loop()
{
  if( bLastRead )
  {
    bLastRead= false;
    if(bInitSearch) ReadOneWireDS18xTemp();
  }

  // Sleep until the next interrupt happens
  __WFI();
}
