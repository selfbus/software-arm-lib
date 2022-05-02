/*
 * userEeprom.h
 *
 *  Created on: 19.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USEREEPROM_H_
#define SBLIB_EIB_USEREEPROM_H_

#include <sblib/eib/memory.h>
#include <sblib/eib/types.h>
#include <sblib/platform.h>
#include <sblib/bits.h>

/** number of interface objects supported */
#define INTERFACE_OBJECT_COUNT 8

class BcuBase;

/**
 * The user EEPROM
 * @details Can be accessed by name, like userEeprom.manuDataH() and as an array, like
 *          userEeprom[addr]. Please note that the @ref startAddress is subtracted.
 *          E.g. userEeprom[0x107] is the correct address for userEeprom.version() not userEeprom[0x07].
 *
 * @note see KNX Spec. 2.1
 *       - BCU 1 (256 bytes) : 9/4/1 3.1.10.3.1 p.13ff
 *       - BCU 2 (992 bytes) : 9/4/1 5.1.2.12.5 p.45ff
 *       - BIM112            : not in Spec. 2.1 some information in 06 Profiles 4.2.10 p.36
 */
class UserEeprom : public Memory
{
public:
    UserEeprom() = delete;
    UserEeprom(BcuBase* bcu, unsigned int start, unsigned int size, unsigned int flashSize);
	~UserEeprom() = default;

	__attribute__ ((aligned (FLASH_RAM_BUFFER_ALIGNMENT))) byte *userEepromData; // must be word aligned, otherwise iapProgram will fail
	BcuBase* bcu;

    virtual byte& optionReg() const = 0;
    virtual byte& manuDataH() const = 0;
    virtual byte& manuDataL() const = 0;
    virtual byte& manufacturerH() const = 0;
    virtual byte& manufacturerL() const = 0;
    virtual byte& deviceTypeH() const = 0;
    virtual byte& deviceTypeL() const = 0;
    virtual byte& version() const = 0;
    virtual byte& checkLimit() const = 0;
    virtual byte& appPeiType() const = 0;
    virtual byte& syncRate() const = 0;
    virtual byte& portCDDR() const = 0;
    virtual byte& portADDR() const = 0;
    virtual byte& runError() const = 0;
    virtual byte& routeCnt() const = 0;
    virtual byte& maxRetransmit() const = 0;
    virtual byte& confDesc() const = 0;
    virtual byte& assocTabPtr() const = 0;
    virtual byte& commsTabPtr() const = 0;
    virtual byte& usrInitPtr() const = 0;
    virtual byte& usrProgPtr() const = 0;

    virtual byte& addrTabSize() const = 0;
    virtual byte* addrTab() const = 0;

    /**
     * Access the user EEPROM like an ordinary array. The @ref startAddress is subtracted
     * when accessing the EEPROM. So use userEeprom[0x107] to access e.g. userEeprom.version.
     *
     * @param address - the address of the data byte to access.
     * @return The data byte.
     */
    byte& operator[](uint32_t address) override;
    uint8_t getUInt8(uint32_t address) const override;
    uint16_t getUInt16(uint32_t address) const override;

    /**
     * Mark the user EEPROM as modified. The EEPROM will be written to flash when the
     * bus is idle, all telegrams are processed, and no direct data connection is open.
     */
    void modified();

    /**
     * Test if the user EEPROM is modified.
     */
    bool isModified() const;

    uint32_t flashSize() const;

    unsigned int numEepromPages() const;
    byte* lastEepromPage() const;
    byte* flashSectorAddress() const;

    void writeUserEeprom();
    void readUserEeprom();
    unsigned int writeUserEepromTime = 0;
    bool userEepromModified = false;

protected:
    /**
     * Finds the last valid page in the flash sector.
     *
     * @details The search is done backwards from the end of the mcu flash in  @ref FLASH_SECTOR_SIZE steps.
     *
     * @return If successful: number of the last valid flash page, otherwise 0
     */
    byte* findValidPage();

    const unsigned int userEepromFlashSize;
};


#endif /* SBLIB_EIB_USEREEPROM_H_ */
