/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_BCU1_H_
#define SBLIB_EIB_USERRAM_BCU1_H_

#include <stdint.h>
#include <sblib/eib/userRam.h>

/**
 * The BCU 1 user RAM
 * @details Can be accessed by name, like userRam.status() and as an array, like
 *          userRam[addr]. Please note that a possible  @ref startAddress is subtracted.
 *
 * @note see KNX Spec. 2.1 - BCU 1 (256 bytes) : 9/4/1 3.1.10.2 p.11ff
 */
class UserRamBCU1 : public UserRam
{
public:
	UserRamBCU1() : UserRam(0, 0x100, 3) {}


    static const uint32_t _runStateOffset = 0x61; ///\todo properties still need this to be public
	/**
     * BCU 1 device control (address 0x62). See enum @ref DeviceControl
     */
    static const uint32_t _deviceControlOffset = 0x62; ///\todo properties still need this to be public
	static const uint32_t _peiTypeOffset = 0x63; ///\todo properties still need this to be public
	static const uint32_t _user2Offset = 0xC8; ///\todo properties still need this to be public

	virtual uint8_t& deviceControl() const override { return userRamData[_deviceControlOffset]; }
	virtual uint8_t& peiType() const override{ return userRamData[_peiTypeOffset]; }

protected:
	UserRamBCU1(unsigned int start, unsigned int size, unsigned int shadowSize) : UserRam(start, size, shadowSize) {}

    virtual uint32_t statusOffset() const override {return _statusOffset;}
    virtual uint32_t runStateOffset() const override {return _runStateOffset;}

    /**
     * BCU 1 system status (address 0x60). See enum @ref BcuStatus
     */
    static const uint32_t _statusOffset = 0x60; //!< this is questionable: "real status address 0x0100" ?? is it really?


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
		     *         is sued for com objects as well. Therefore the real
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
		     *         is used for com objects as well. Therefore the real
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
			byte user2[0x100 - 0xc8];
		};
		byte userRamData[0x100];
	} data;
#endif
};

#endif /* SBLIB_EIB_USERRAM_BCU1_H_ */
