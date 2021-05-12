/*
 *  Copyright (c) 2014 Martin Glueck <martin@mangari.org>
 *  Copyright (c) 2021 Stefan Haller
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef BOOT_DESCRIPTOR_BLOCK_H_
#define BOOT_DESCRIPTOR_BLOCK_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define BL_IDENTITY			 0x1055		// Version 0.55
#define BL_ID_STRING         "[SB KNX BL ]"
#define BOOT_BLOCK_DESC_SIZE 0x100		// 1 flash page
#define BOOT_BLOCK_PAGE   ((FIRST_SECTOR / BOOT_BLOCK_DESC_SIZE) - 1)
#define BOOT_DSCR_ADDRESS 	(FIRST_SECTOR - BOOT_BLOCK_DESC_SIZE)	// Descriptor uses last page of bootloader

#ifdef DUMP_TELEGRAMS
	#define	BL_FEATURES  	 0x8100
	#define FIRST_SECTOR	 0x3000		// where the application starts (BL size)
#else
	#define BL_FEATURES		 0x0100		// Feature list of BL
	#define FIRST_SECTOR	 0x2000		// where the application starts (BL size)
#endif

typedef struct
{
    unsigned int startAddress;
    unsigned int endAddress;
    unsigned int crc;
    unsigned int appVersionAddress;
}__attribute__ ((aligned (256))) AppDescriptionBlock;

unsigned int checkApplication(AppDescriptionBlock * block);

unsigned char * getAppVersion(AppDescriptionBlock * block);
unsigned char * getFWstartAddress(AppDescriptionBlock * block);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_DESCRIPTOR_BLOCK_H_ */
