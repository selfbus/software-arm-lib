/*
 * userEeprom.h
 *
 *  Created on: 19.11.2021
 *      Author: dridders
 */

#ifndef SBLIB_EIB_USEREEPROM_H_
#define SBLIB_EIB_USEREEPROM_H_

#include <sblib/eib/types.h>
#include <sblib/platform.h>


/** number of interface objects supported */
#define INTERFACE_OBJECT_COUNT 8

class BcuBase;

class UserEeprom
{
public:
	UserEeprom(BcuBase* bcu, int start, int size, int flashSize);
	virtual ~UserEeprom() = default;

	byte *userEepromData;
	BcuBase* bcu;

	virtual byte& manufacturerH() const = 0;
	virtual byte& manufacturerL() const = 0;
	virtual byte& deviceTypeH() const = 0;
	virtual byte& deviceTypeL() const = 0;
	virtual byte& version() const = 0;
	virtual byte& appPeiType() const = 0;
	virtual byte& portADDR() const = 0;
	virtual byte& runError() const = 0;
    virtual byte& addrTabSize() const = 0;
    virtual byte& assocTabPtr() const = 0;
    virtual byte* addrTab() const = 0;
    virtual byte& commsTabPtr() const = 0;

    /**
     * Access the user EEPROM like an ordinary array. The start address is subtracted
     * when accessing the EEPROM. So use userEeprom[0x107] to access userEeprom.version.
     *
     * @param idx - the index of the data byte to access.
     * @return The data byte.
     */
    byte& operator[](int idx);
    unsigned char getUInt8(int idx);

    /**
     * Access the user EEPROM like an ordinary array. The start address is subtracted
     * when accessing the EEPROM. So use userEeprom[0x107] to access userEeprom.version.
     *
     * @param idx - the index of the 16 bit data to access.
     * @return The 16bit as unsigned int.
     */
    unsigned short getUInt16(int idx);

    /**
     * Mark the user EEPROM as modified. The EEPROM will be written to flash when the
     * bus is idle, all telegrams are processed, and no direct data connection is open.
     */
    void modified();

    /**
     * Test if the user EEPROM is modified.
     */
    bool isModified() const;

    const int userEepromStart;
    const int userEepromSize;
    const int userEepromEnd;
    const int userEepromFlashSize;

    virtual int numEepromPages() const;
    virtual byte* lastEepromPage() const;
    virtual byte* flashSectorAddress() const;

    void writeUserEeprom();
    void readUserEeprom();
    unsigned int writeUserEepromTime = 0;
    bool userEepromModified = false;

protected:
    byte* findValidPage();
};

inline byte& UserEeprom::operator[](int idx)
{
    return userEepromData[idx];
}

inline unsigned char UserEeprom::getUInt8(int idx)
{
    return userEepromData[idx];
}

inline unsigned short UserEeprom::getUInt16(int idx)
{
    return (userEepromData[idx] << 8) | userEepromData[idx+1];
}

inline void UserEeprom::modified()
{
    userEepromModified = 1;
    writeUserEepromTime = 0;
}

inline bool UserEeprom::isModified() const
{
    return userEepromModified;
}

#endif /* SBLIB_EIB_USEREEPROM_H_ */
