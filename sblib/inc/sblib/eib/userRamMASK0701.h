/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_MASK0701_H_
#define SBLIB_EIB_USERRAM_MASK0701_H_

#include <sblib/eib/userRamBCU2.h>

/**
 * The user RAM.
 *
 * The user RAM can be accessed by name, like userRam.status and as an array, like
 * userRam[addr]. Please note that the start address of the RAM is subtracted.
 */
class UserRamMASK0701 : public UserRamBCU2
{
public:
	UserRamMASK0701() : UserRamBCU2(0, 0x304) {}

protected:
	UserRamMASK0701(int start, int size) : UserRamBCU2(start, size) {}

#if 0
	union __attribute__ ((aligned (4)))
	{
		struct {
		    /**
		     * 0x0000: Application program data.
		     */
			byte user1[0x60];

		    /**
		     * 0x0060: BCU1 system status. See enum BcuStatus below.
		     *         In some modes (BCU2 as BCU1) this part of the RAM
		     *         is sued for com objects as well. Therefor the real
		     *         status is at the end of the user ram.
		     */
			byte _status;

		    /**
		     * 0x0061: BCU2 application run state.
		     *
		     * 0 = the application program is halted
		     * 1 = the program is running
		     * 2 = the program is ready but not running
		     * 3 = the program is terminated
		     *
		     *         In some modes (BCU2 as BCU1) this part of the RAM
		     *         is sued for com objects as well. Therefor the real
		     *         runState is at the end of the user ram.
		     */
			byte _runState;

		    /**
		     * 0x0062: BCU2 Device control, see enum DeviceControl below.
		     *
		     * Bit 0: set if the application program is stopped.
		     * Bit 1: a telegram with our own physical address was received.
		     * Bit 2: send a memory-response telegram automatically on memory-write.
		     */
			byte deviceControl;

		    /**
		     * 0x0063: PEI type. This is the type of the physical external interface
		     *         that is connected to the device. It is not used in Selfbus programs.
		     */
			byte peiType;

		    /**
		     * 0x0064: Reserved for system software.
		     */
			byte reserved[0x64];

		    /**
		     * 0x00C8: Application program data.
		     */
			byte user2[0x304 - 0xc8];
		};
		byte userRamData[0x304];
	} data;
#endif
};

#endif /* SBLIB_EIB_USERRAM_H_ */
