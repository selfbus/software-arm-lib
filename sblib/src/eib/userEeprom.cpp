/*
 * userEeprom.cpp
 *
 *  Created on: 19.11.2021
 *      Author: dridders
 */

#include <sblib/eib/userEeprom.h>
#include <sblib/internal/iap.h>
#include <sblib/eib/bcu_base.h>
#include <sblib/eib/bus.h>
#include <cstring>

uint32_t UserEeprom::flashSize() const
{
    return (userEepromFlashSize);
}

unsigned int UserEeprom::numEepromPages() const
{
	return FLASH_SECTOR_SIZE / flashSize();
}

byte* UserEeprom::lastEepromPage() const
{
	return flashSectorAddress() + flashSize() * (numEepromPages() - 1);
}

byte* UserEeprom::flashSectorAddress() const
{
	return (FLASH_BASE_ADDRESS + iapFlashSize() - FLASH_SECTOR_SIZE);
}

byte* UserEeprom::findValidPage()
{
    byte* firstPage = FLASH_BASE_ADDRESS + iapFlashSize() - FLASH_SECTOR_SIZE;
    byte* page = lastEepromPage();

    while (page >= firstPage)
    {
        if (page[size() - 1] != 0xff)  ///\todo check more then  only one byte for 0xff (empty flash)
            return page;

        page -= userEepromFlashSize;
    }

    return 0; // no valid page found
}

void UserEeprom::readUserEeprom()
{
    byte* page = findValidPage();

    if (page)
        memcpy(userEepromData, page, size());
    else
        memset(userEepromData, 0, size()); ///\todo should filling with zeros indicate a readError? if yes, then it should be somewhere reported

    modified(false);
}

void UserEeprom::writeUserEeprom()
{
    if (!isModified())
    {
        return;
    }

    noInterrupts();

    byte* page = findValidPage();
    if (page == lastEepromPage())
    {
        page = flashSectorAddress();
    }
    else if (page)
        page += userEepromFlashSize;
    else{
    	page = flashSectorAddress();
    }

    if (page == flashSectorAddress())
    {
        // Erase the sector
        int sectorId = iapSectorOfAddress(page);
        IAP_Status rc = iapEraseSector(sectorId);
        if (rc != IAP_SUCCESS)
        {
            fatalError(); // erasing failed
        }
    }

    userEepromData[size() - 1] = 0; // mark the page as in use

    IAP_Status rc;

    for (unsigned int i = 0; i < size(); i += 1024)
    {
    	int chunk = size() - i;
    	if (chunk > 1024)
    	{
    		chunk = 1024;
    	}
    	rc = iapProgram(page + i, userEepromData + i, chunk);
    	if (rc != IAP_SUCCESS)
    	{
            fatalError(); // flashing failed
    	}
    }

    modified(false);
    interrupts();
}

UserEeprom::UserEeprom(unsigned int start, unsigned int size, unsigned int flashSize) :
		Memory(start, size),
		userEepromData(new byte[size]()),
		userEepromFlashSize(flashSize)
{
    readUserEeprom();
}

byte& UserEeprom::operator[](uint32_t address)
{
    normalizeAddress(&address);
    return userEepromData[address];
}


uint8_t UserEeprom::getUInt8(uint32_t address) const
{
    normalizeAddress(&address);
    if (address > (size() - 1))
    {
        return 0;
    }
    return userEepromData[address];
}

uint16_t UserEeprom::getUInt16(uint32_t address) const
{
    normalizeAddress(&address);
    if ((address + 1) > (size() - 1))
    {
        return 0;
    }
    return makeWord(userEepromData[address], userEepromData[address + 1]);
}

void UserEeprom::modified(bool newModified)
{
    userEepromModified = newModified;
    if (userEepromModified)
    {
        writeUserEepromTime = millis() + 50;
    }
    else
    {
        writeUserEepromTime = 0;
    }
}

bool UserEeprom::writeDelayElapsed() const
{
    bool elapsed = userEepromModified;
    elapsed |= ((int)millis() - (int)writeUserEepromTime) > 0;
    return (elapsed);
}

bool UserEeprom::isModified() const
{
    return userEepromModified;
}
