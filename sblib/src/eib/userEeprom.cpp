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

inline unsigned int UserEeprom::numEepromPages() const
{
	return FLASH_SECTOR_SIZE / userEepromFlashSize;
}

inline byte* UserEeprom::lastEepromPage() const
{
	return flashSectorAddress() + userEepromFlashSize * (numEepromPages() - 1);
}

inline byte* UserEeprom::flashSectorAddress() const
{
	return (FLASH_BASE_ADDRESS + iapFlashSize() - FLASH_SECTOR_SIZE);
}

byte* UserEeprom::findValidPage()
{
    byte* firstPage = FLASH_BASE_ADDRESS + iapFlashSize() - FLASH_SECTOR_SIZE;
    byte* page = lastEepromPage();

    while (page >= firstPage)
    {
        if (page[userEepromSize - 1] != 0xff)  ///\todo check more then  only one byte for 0xff (empty flash)
            return page;

        page -= userEepromFlashSize;
    }

    return 0; // no valid page found
}

void UserEeprom::readUserEeprom()
{
    byte* page = findValidPage();

    if (page)
        memcpy(userEepromData, page, userEepromSize);
    else
        memset(userEepromData, 0, userEepromSize); // TODO should filling with zeros indicate a readError? if yes, then it should be somewhere reported

    userEepromModified = false;
}

void UserEeprom::writeUserEeprom()
{
    if (!userEepromModified)
        return;

    // Wait for an idle bus and then disable the interrupts
    while (!bcu->bus->idle())
        ;
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

    userEepromData[userEepromSize - 1] = 0; // mark the page as in use

    IAP_Status rc;

    for (unsigned int i = 0; i < userEepromSize; i += 1024)
    {
    	int chunk = userEepromSize - i;
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

    interrupts();
    userEepromModified = 0;
}

UserEeprom::UserEeprom(BcuBase* bcu, unsigned int start, unsigned int size, unsigned int flashSize) :
		userEepromData(new byte[size]), bcu(bcu), userEepromStart(start), userEepromSize(size), userEepromEnd(start+size-1), userEepromFlashSize(flashSize)
{
    readUserEeprom();
}

byte& UserEeprom::operator[](unsigned int idx)
{
    idx -= userEepromStart;
    return userEepromData[idx];
}

