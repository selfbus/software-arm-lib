/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_H_
#define SBLIB_EIB_USERRAM_H_

/**
 * BCU status bits for userRam.status (at 0x0060).
 * See BCU1 / BCU2 help for detailed description.
 */
enum BcuStatus
{
    BCU_STATUS_PROG= 0x01,    //!< BCU status: programming mode: 0=normal mode, 1=programming mode
    BCU_STATUS_LL  = 0x02,    //!< BCU status: link layer mode (1), or bus monitor mode (0)
    BCU_STATUS_TL  = 0x04,    //!< BCU status; transport layer enabled
    BCU_STATUS_AL  = 0x08,    //!< BCU status: application layer enabled
    BCU_STATUS_SER = 0x10,    //!< BCU status: serial PEI enabled
    BCU_STATUS_USR = 0x20,    //!< BCU status: application program enabled
    BCU_STATUS_DWN = 0x40,    //!< BCU status: download mode enabled
    BCU_STATUS_PARITY = 0x80  //!< BCU status: parity bit: even parity for bits 0..6)
};

/**
 * Device control flags, for userRam.deviceControl
 */
enum DeviceControl
{
    DEVCTRL_APP_STOPPED = 0x01,      //!< the application program is stopped.
    DEVCTRL_OWN_ADDR_IN_USE = 0x02,  //!< a telegram with our own physical address was received.
    DEVCTRL_MEM_AUTO_RESPONSE = 0x04 //!< send a memory-response telegram automatically on memory-write.
};

/**
 * The user RAM.
 *
 * The user RAM can be accessed by name, like userRam.status and as an array, like
 * userRam[addr]. Please note that the start address of the RAM is subtracted.
 */
class UserRam
{
public:
	UserRam(unsigned int start, unsigned int size, unsigned int shadowSize) : userRamData(new byte[size + shadowSize]), userRamStart(start), userRamSize(size), userRamEnd(start+size) {};

    byte status; // real status 0x0100
    byte runState; // 0x0101

    byte& operator[](unsigned int idx);

    byte* userRamData = 0;
	virtual byte& deviceControl() const = 0;
	virtual byte& peiType() const = 0;

	unsigned int userRamStart;
    const unsigned int userRamSize;
    unsigned int userRamEnd;

    void setUserRamStart(const unsigned int ramStart) { userRamStart = ramStart; userRamEnd = ramStart + userRamSize; }
};

inline byte& UserRam::operator[](unsigned int idx)
{
	idx -= userRamStart;
    return userRamData[idx];
}


#endif /* SBLIB_EIB_USERRAM_H_ */
