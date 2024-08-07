/*
 *  iap_emu.h - Emulation of the In App Programming functions
 *
 *  Copyright (c) 2014 Martin Glueck <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef IAP_EMU_H_
#define IAP_EMU_H_

#ifdef __cplusplus
extern "C"
{
#endif

enum IAP_Function
{
    I_PREPARE = 0,
    I_ERASE = 1,
    I_BLANK_CHECK = 2,
    I_RAM2FLASH = 3,
    I_COMPARE = 4,
    I_READ_UID = 5
};

extern int iap_calls[6];
void IAP_Init_Flash(unsigned char value);
void IAP_Call (uintptr_t * cmd, uintptr_t * stat);


// Size of a flash sector: 4k
#define SECTOR_SIZE  0x1000

// Size for smaller LPC1114 simulated flash: 32k (8 * 4k)
// #define FLASH_SIZE  0x8000

// Size for the simulated flash: 64k (16 * 4k), value for LPC1115
#define FLASH_SIZE  0x10000


#ifdef __cplusplus
}
#endif

#endif /* IAP_EMU_H_ */
