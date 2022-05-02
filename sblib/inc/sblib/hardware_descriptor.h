/**************************************************************************//**
 * @addtogroup
 * @defgroup
 * @ingroup
 * @brief
 * @details
 *
 * @{
 *
 * @file   hardware_descriptor.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#ifndef SBLIB_HARDWARE_DESCRIPTOR_H_
#define SBLIB_HARDWARE_DESCRIPTOR_H_

#include <stdint.h>

uint32_t hwGuid();
uint8_t hwDescriptorSize();
uint16_t hwDescriptorVersion();
uint16_t hwHardwareID();
uint32_t hwOptions();
uint32_t hwPinEibTx();
uint32_t hwPinEibRx();
uint32_t hwPinProgButton();
uint32_t hwPinBusVoltage();
uint32_t hwPinInfo();
uint32_t hwPinRun();
uint8_t hwInvertProgButton();

#endif /*SBLIB_HARDWARE_DESCRIPTOR_H_*/

/** @}*/
