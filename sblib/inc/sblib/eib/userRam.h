/*
 * userRam.h
 *
 *  Created on: 18.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USERRAM_H_
#define SBLIB_EIB_USERRAM_H_

#include <stdint.h>
#include <sblib/eib/memory.h>

/**
 * BCU status bits for @ref status()
 * See BCU1 / BCU2 help for detailed description.
 */
enum BcuStatus
{
    BCU_STATUS_PROGRAMMING_MODE  = 0x01, //!< Programming mode: 0=normal mode, 1=programming mode
    BCU_STATUS_LINK_LAYER        = 0x02, //!< Link layer mode (1), or bus monitor mode (0)
    BCU_STATUS_TRANSPORT_LAYER   = 0x04, //!< Transport layer enabled
    BCU_STATUS_APPLICATION_LAYER = 0x08, //!< Application layer enabled
    BCU_STATUS_SERIAL_PEI        = 0x10, //!< Serial PEI enabled
    BCU_STATUS_USER_MODE         = 0x20, //!< Application program enabled
    BCU_STATUS_DOWNLOAD_MODE     = 0x40, //!< Download mode enabled
    BCU_STATUS_PARITY            = 0x80, //!< Parity bit: even parity for bits 0..6)
};

/**
 * Device control flags, for @ref deviceControl()
 */
enum DeviceControl
{
    DEVCTRL_APPLICATION_STOPPED = 0x01, //!< The application program is stopped.
    DEVCTRL_OWN_ADDRESS_IN_USE  = 0x02, //!< A telegram with our own physical address was received.
    DEVCTRL_MEM_AUTO_RESPONSE   = 0x04, //!< Send a memory-response telegram automatically on memory-write.
};

/**
 * The BCU user RAM
 * @details Can be accessed by name, like userRam.status() and as an array, like
 *          userRam[addr]. Please note that a possible  @ref startAddress is subtracted.
 *
 * @note see KNX Spec. 2.1
 *       - BCU 1 (256 bytes) : 9/4/1 3.1.10.2 p.11ff
 *       - BCU 2 (992 bytes) : 9/4/1 5.1.2.12 p.43ff
 *       - BIM112            : not in Spec. 2.1 some information in 06 Profiles 4.2.10 p.36
 */
class UserRam : public Memory
{
public:
    UserRam() = delete;
    ~UserRam() = default;
    UserRam(uint32_t start, uint32_t size, uint32_t shadowSize);

    /**
     * System status. See enum @ref BcuStatus
     * @details In some modes (BCU2 in compatibility mode as BCU1) this part of the RAM
     *          is used for communication objects as well.
     *          Therefore the real status is at the end of the user ram.
     */
    uint8_t& status();
    uint8_t& runState();

    /**
     * Device control, see enum @ref DeviceControl below
     * @details - Bit 0: set if the application program is stopped.
     *          - Bit 1: set if a telegram with our own physical address was received.
     *          - Bit 2: send a memory-response telegram automatically on memory-write.
     *
     * @return Pointer to the device control byte
     */
	virtual uint8_t& deviceControl() const = 0;

    /**
     * PEI type
     * @details This is the type of the physical external interface that is connected to the device.
     * @warning not implemented
     */
	virtual uint8_t& peiType() const = 0;

	void setUserRamStart(const uint32_t &newRamStart);

	uint8_t& operator[](uint32_t address) override;
    uint8_t getUInt8(uint32_t address) const override;
    uint16_t getUInt16(uint32_t address) const override;

    void cpyFromUserRam(uint32_t address, unsigned char * buffer, uint32_t count);
    void cpyToUserRam(uint32_t address, unsigned char * buffer, uint32_t count);

    bool isStatusAddress(uint32_t address) const;

    uint8_t* userRamData = 0;

protected:
    virtual uint32_t statusOffset() const = 0;
    virtual uint32_t runStateOffset() const = 0;

    uint8_t _status = 0; //!< some BCU 1 & 2 (e.g. out8-bcu1) override we real status @ 0x060 with comObjects in RAM, so we place it outside of the real RAM
    uint8_t _runState = 0; //!< some BCU 1 & 2 (e.g. out8-bcu1) override we real runState @ 0x061 with comObjects in RAM, so we place it outside of the real RAM
    // static const uint32_t _runStateOffset = 0;

private:

    uint32_t shadowSize;
};


#endif /* SBLIB_EIB_USERRAM_H_ */
