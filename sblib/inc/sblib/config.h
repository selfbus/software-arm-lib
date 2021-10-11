/**************************************************************************//**
 * @file     config.h
 * @brief    set pre-processor symbols which apply to all sblib build-configs
 * @date     2021/04/02
 *
 * @note     for serial output it is recommended to use PIO2_7 and PIO2_8 at high baud rate e.g 1.5MB
 *           serial.setRxPin(PIO2_7);
 *           serial.setTxPin(PIO2_8);
 *           serial.begin(1500000);  1.5Mbaud
 *           should be set in your app, serial object is defined in serial.h
 ******************************************************************************/

#ifndef SBLIB_CONFIG_H_
#define SBLIB_CONFIG_H_

/**************************************************************************//**
 *
 * Things to configure in the sblib
 *
 ******************************************************************************/
// #define ROUTER /// \todo ROUTER not really implemented, Lib will be compiled for usage in a router- Phy Addr 0.0.0 is allowed



/**************************************************************************//**
 *
 * Debugging stuff follows below
 *
 ******************************************************************************/
// for every debugging or dumping #define, make sure to add a #undef below, so it won't make it into the release version
//#define DEBUG_BUS           //!> enable dumping of state machine interrupt data e.g timer values, mapping of ports in serial.cpp
//#define DEBUG_BUS_BITLEVEL  //!> extension used with DEBUG_BUS to dump interrupt of each bit - use with care due to easy overflow of the trace buffer

//to avoid trace buffer overflow DUMP_TELEGRAMS should no be used in parallel to DEBUG_BUS/DEBUG_BUS_BITLEVEL
//#define DUMP_TELEGRAMS  //!> dump rx and tx telegrams, incl received ack over serial interface
//#define DUMP_COM_OBJ    //!> dump object handling information on app-server level over serial interface
//#define DUMP_MEM_OPS    //!> dump APCI_MEMORY_WRITE_PDU and APCI_MEMORY_READ_PDU over serial interface
//#define DUMP_SERIAL     //!> dump KNX-serialnumber over serial interface
//#define DUMP_PROPERTIES //!> dump interface object and property handling over serial interface

/// \todo following #defines should be moved to this config.h file
// #define BH1750_DEBUG /// \todo found in bh1750.h
// #define LOGGING      /// \todo found in bh1750.h
// #define DEBUG_ACTIVE /// \todo found in debug.cpp

/**************************************************************************//**
 *
 * Change things below only, if you know what you are doing.
 *
 ******************************************************************************/

// remove any debugging and dumping stuff from release versions
// for every debugging or dumping #define above, make sure to add a #undef below
#ifndef DEBUG
#   undef DEBUG_BUS
#   undef DEBUG_BUS_BITLEVEL
#   undef DUMP_TELEGRAMS
#   undef DUMP_COM_OBJ
#   undef DUMP_MEM_OPS
#   undef DUMP_SERIAL
#   undef DUMP_PROPERTIES
#   undef LOGGING
#   undef BH1750_DEBUG
#   undef DEBUG_ACTIVE
#endif

// make sure if DEBUG_BUS_BITLEVEL is defined also DEBUG_BUS is defined
#if defined(DEBUG_BUS_BITLEVEL) && !defined(DEBUG_BUS)
#  //define DEBUG_BUS
#  warning "DEBUG_BUS_BITLEVEL, can only be used together with DEBUG_BUS"
#endif

#endif /* SBLIB_CONFIG_H_ */
