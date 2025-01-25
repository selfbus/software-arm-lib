/**************************************************************************//**
 * @addtogroup SBLIB Selfbus KNX-Library sblib
 * @defgroup SBLIB_CORE CORE
 * @ingroup SBLIB
 * @brief    set pre-processor symbols which apply to all sblib build-configs
 * @note     for serial debugging output it is recommended to use e.g. ID1/2 PINs
 *           PIO2_7 and PIO2_8 at high baud rate e.g 1.5MB
 *           define below SERIAL_RX_PIN and SERIAL_TX_PIN
 *           or set it in your application
 *           serial.setRxPin(PIO2_7);
 *           serial.setTxPin(PIO2_8);
 *           serial.begin(1500000);  1.5Mbaud
 *           serial object is defined in serial.h
 *
 * @{
 *
 * @file   libconfig.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @author Horst Rauch Copyright (c) 2021, 2025
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#ifndef SBLIB_LIBCONFIG_H_
#define SBLIB_LIBCONFIG_H_

/**************************************************************************//**
 * Things to configure in the sblib
 ******************************************************************************/

#if defined (__LPC11XX__)
#if defined ( SERIAL_ON_SWD)
#   define SERIAL_RX_PIN PIO1_6 //!< @ swd/jtag connector
#   define SERIAL_TX_PIN PIO1_7 //!< @ swd/jtag connector
#elif defined (SERIAL_ON_SV2)
#   define SERIAL_RX_PIN PIO3_1 //!< on a TS_ARM Controller
#   define SERIAL_TX_PIN PIO3_0 //!< on a TS_ARM Controller
#else
#   define SERIAL_RX_PIN PIO2_7 //!< on a 4TE-ARM Controller pin 1 on connector SV3 (ID_SEL)
#   define SERIAL_TX_PIN PIO2_8 //!< on a 4TE-ARM Controller pin 2 on connector SV3 (ID_SEL)
#endif
#endif

/** @def SERIAL_SPEED baudrate e.g. 115200, 230400, 576000 serial port should run for debugging */
//#define SERIAL_SPEED 115200
//#define SERIAL_SPEED 576000
//#define SERIAL_SPEED 1500000

/** @def INCLUDE_SERIAL library should open serial port for us */
//#define INCLUDE_SERIAL

//#define ROUTER /// \todo create a new class derived from BcuDefault to build a ROUTER

/******************************************************************************
 * Things for some HW modifications on theTE4 BCU board for ADC conversion
 * and Bus voltage monitoring.
 *  - Pin for ext Vref e.g. LM4040 2.5 - AD0/LT8 or AD1/IO9
 *  - replacement of D2 (3V6) by diode (Bat41 or 1n4148, Vf ~0.5V @If 100uA) to Vcc/+3V3
 *
 */
#if defined (VREF_LT8)
# 	define	PIN_EXT_VREF_AD PIN_EXT_VREF_AD0
# 	define	EXTVREF_ADC EXTVREF_ADC0
#else
# 	define	PIN_EXT_VREF_AD PIN_EXT_VREF_AD1
# 	define	EXTVREF_ADC EXTVREF_ADC1
#endif


/**************************************************************************//**
 * Debugging stuff follows below
 *
 * for every debugging or dumping #define, make sure to add a #undef below,
 * so it won't make it into the release version
 ******************************************************************************/

/** @def DEBUG_BUS enable dumping of state machine interrupt data e.g timer values, mapping of ports in serial.cpp */
//#define DEBUG_BUS

/** @def DEBUG_BUS_VOLTAGE  enable dumping of bus voltage monitoring and measurements at start up (in main) and run level (in bcu_default) */
//#define DEBUG_BUS_VOLTAGE

/** @def DEBUG_BUS_BITLEVEL extension used with DEBUG_BUS to dump interrupt of each bit - use with care due to easy overflow of the trace buffer*/
//#define DEBUG_BUS_BITLEVEL

/** @def BUSMONITOR enables bus monitoring: dump of all bus traffic incl timing info, no sending, no processing of rx-telegrams */
//#define BUSMONITOR

/**
 *  @def DUMP_TELEGRAMS dump rx and tx telegrams, incl received ack and timing info over serial interface
 *  @warning to avoid trace buffer overflow @ref DUMP_TELEGRAMS should not be used in parallel with @ref DEBUG_BUS or @ref DEBUG_BUS_BITLEVEL
 */
//#define DUMP_TELEGRAMS

/** @def PIO_FOR_TEL_END_IND toggle defined PIO pin at the end of a received telegram */
//#define PIO_FOR_TEL_END_IND (PIO1_4)

/** @def DUMP_COM_OBJ dump object handling information on app-server level over serial interface */
//#define DUMP_COM_OBJ

/** @def DUMP_MEM_OPS dump @ref APCI_MEMORY_WRITE_PDU and @ref APCI_MEMORY_READ_PDU over serial interface */
//#define DUMP_MEM_OPS

/** @def DUMP_PROPERTIES dump interface object and property handling over serial interface */
//#define DUMP_PROPERTIES

/** @def DUMP_TL4 dump transport layer 4 protocol handling over serial interface */
//#define DUMP_TL4

/// \todo following #defines should be moved to this libconfig.h file
// IAP_EMULATION        /// \todo from platform.h & analog_pin.cpp (used for catch-unit tests of the sblib)
// DEBUG                /// \todo from utils.h
// SERIAL_WRITE_DIRECT  /// \todo from serial.cpp
// BH1750_DEBUG         /// \todo from in bh1750.h
// LOGGING              /// \todo from in bh1750.h
// DEBUG_ACTIVE         /// \todo from in debug.cpp

/**************************************************************************//**
 *
 * Change things below only, if you know what you are doing.
 *
 * for every debugging or dumping #define above, make sure to add a #undef below
 *
 ******************************************************************************/

// remove any debugging and dumping stuff from release versions
#ifndef DEBUG
#   undef DEBUG_BUS
#	undef DEBUG_BUS_VOLTAGE
#   undef DEBUG_BUS_BITLEVEL
#   undef BUSMONITOR
#   undef DUMP_TELEGRAMS
#	undef PIO_FOR_TEL_END_IND
#   undef DUMP_COM_OBJ
#   undef DUMP_MEM_OPS
#   undef DUMP_PROPERTIES
#   undef DUMP_TL4
#   undef LOGGING
#   undef BH1750_DEBUG
#   undef DEBUG_ACTIVE
#   undef INCLUDE_SERIAL
#endif

// make sure if DEBUG_BUS_BITLEVEL is defined also DEBUG_BUS is defined
#if defined(DEBUG_BUS_BITLEVEL) && !defined(DEBUG_BUS)
#  define DEBUG_BUS
#  warning "DEBUG_BUS_BITLEVEL, can only be used together with DEBUG_BUS"
#endif

// make sure if BUSMONITOR is defined also DUMP_TELEGRAMS is defined
#if defined(BUSMONITOR) && !defined(DUMP_TELEGRAMS)
#  define DUMP_TELEGRAMS
#  warning "BUSMONITOR can only be used together with DUMP_TELEGRAMS"
#endif

//to avoid trace buffer overflow DUMP_TELEGRAMS should not be used in parallel with DEBUG_BUS or DEBUG_BUS_BITLEVEL
#if defined(DUMP_TELEGRAMS) && (defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL))
#   warning "DUMP_TELEGRAMS should not be used in parallel with DEBUG_BUS or DEBUG_BUS_BITLEVEL"
#endif

// list here all defines which need the serial port
#if defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL) || defined(DUMP_TELEGRAMS) || defined(DUMP_COM_OBJ) || \
    defined(DUMP_MEM_OPS) || defined(DUMP_PROPERTIES) || defined(DUMP_TL4) || defined (DEBUG_BUS_VOLTAGE)
#   define INCLUDE_SERIAL
#endif

#if defined(INCLUDE_SERIAL) && !defined(SERIAL_SPEED)
#   warning "Default debugging serial port speed set to 115200"
#   define SERIAL_SPEED 115200
#endif

#endif /* SBLIB_LIBCONFIG_H_ */

/** @}*/
