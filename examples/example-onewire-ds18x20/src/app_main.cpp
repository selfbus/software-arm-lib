/**************************************************************************//**
 * @file    app_main.cpp
 * @brief   A simple application which will scan and read Temperature from DS18x20
 *          family devices using the OneWire (1-Wire) Protocol.
 *          Implementation on 4TE-ARM-Controller. Including OneWire parasite power mode.
 *          1-wire data line pin is PIO1.7
 *
 *          needs BCU1 version of the sblib library
 *
 * @author Erkan Colak <erkanc@gmx.de> Copyright (c) 2015
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

#define DBG_PRINT       0    // Debug print to console

#if DBG_PRINT
#   include <stdio.h>
#endif

#include <sblib/core.h>
#include <sblib/eib/sblib_default_objects.h>
#include <sblib/io_pin_names.h>
#include <sblib/sensors/ds18x20.h>

#define USE_OWN_ROM     1      ///> Use your own extracted rom informations. See SetOwnDs18xDeviceRoms()
#define PORTPIN         PIO1_7 ///> GPIO PIN, where the 1-wire data line is connected
#define PARASITE_POWER  true   ///> Enable the Parasite power (check ds18x20 Datasheet)
#define READ_TIMER 500         ///> Read values timer in milliseconds

#define START_UP_BLINK_DELAY 200 ///> start-up LED_blinking delay in milliseconds

bool bLastRead= false;         ///> Condition to read values if timer reached
bool bInitSearch= false;       ///> Device search state

DS18x20 ds;

#if USE_OWN_ROM
/**
 * Example how to use your own extracted rom.
 */
bool set_Own_Ds18x_Device_Roms()
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

/**
 * Initialize the application.
 */
void setup()
{
    ds.DS18x20Init(PORTPIN, PARASITE_POWER);    // Initialize DS18x20 (1-Wire Protocol will be Initialized automatically)
#if USE_OWN_ROM
    bInitSearch= set_Own_Ds18x_Device_Roms();
#else
    bInitSearch= ds.Search() && ds.m_foundDevices > 0; // Scan the 1-Wire bus for DS18x devices
#endif

    // LED Initialize
    pinMode(PIN_INFO, OUTPUT); // Info LED (yellow)
    pinMode(PIN_RUN, OUTPUT);  // Run  LED (green)
    pinMode(PIN_PROG, OUTPUT); // Prog LED (red)

    // LED Set Initial Value (ON|OFF)
    digitalWrite(PIN_INFO, 1); // Info  LED (yellow), will be toggled with OneWire function (blinks the count of found devices )
    digitalWrite(PIN_RUN, 1);  // Run LED (green), will be toggled with OneWire function (blinks on read success)
    digitalWrite(PIN_PROG, 0); // Prog LED (red), will be used as a heartbeat, that we are still alive

    enableInterrupt(TIMER_32_0_IRQn);                      // Enable the timer interrupt
    timer32_0.begin();                                     // Begin using the timer
    timer32_0.prescaler((SystemCoreClock / 1000) - 1);     // Let the timer count milliseconds
    timer32_0.matchMode(MAT1, RESET | INTERRUPT);          // On match of MAT1, generate an interrupt and reset the timer
    timer32_0.match(MAT1, READ_TIMER);                     // Match MAT1 when the timer reaches this value (in milliseconds)
    timer32_0.start();                                     // Start now the timer
}

/**
 * Read the Temperature from found DS18x Devices.
 */
bool ReadOneWireDS18xTemp()
{
  bool bRet=false;
  if(ds.startConversionAll())                              // Read all temperatures from .m_dsDev
  {
      digitalWrite(PIN_RUN, 1);                            // Switch on the green LED to signal that we read the temperatures
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
        digitalWrite(PIN_INFO, 0); delay(125);             // Switch on<>off shows the count of devices which last read was OK.
        digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));delay(125);
        digitalWrite(PIN_INFO, !digitalRead(PIN_INFO));
      }
      digitalWrite(PIN_RUN, 0);                            // Switch off the green LED. Temperature extraction is done.
  }
#if DBG_PRINT
  if(bRet) printf("\n");
#endif
  return bRet;
}


/**
 * Handler for the timer interrupt.
 */
extern "C" void TIMER32_0_IRQHandler()
{
    // clear the timer interrupt flags, otherwise the interrupt handler is called again immediately after returning.
    timer32_0.resetFlags();
    bLastRead= true;
}

/**
 * The main processing loop while no KNX-application is loaded.
 */
void loop_noapp()
{
    if (bLastRead)
    {
        bLastRead = false;
        if (bInitSearch) ReadOneWireDS18xTemp();
    }

    digitalWrite(PIN_PROG, !digitalRead(PIN_PROG)); // just a little heartbeat, that we are still alive
    delay(500);

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
