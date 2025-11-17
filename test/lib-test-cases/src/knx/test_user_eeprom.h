/*
 * Test class for the userEeprom.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#ifndef TEST_SBLIB_KNX_USEREEPROM_H_
#define TEST_SBLIB_KNX_USEREEPROM_H_

#include <sblib/eib/userEeprom.h>


class TestUserEeprom final : public UserEeprom
{
public:
    TestUserEeprom() = delete;
    using UserEeprom::UserEeprom;
    //TestUserEeprom(unsigned int start, unsigned int size, unsigned int flashSize);
    using UserEeprom::findValidPage;

    [[nodiscard]] uint8_t& optionReg() const override { return userEepromData[0]; }
    [[nodiscard]] uint8_t& manuDataH() const override { return userEepromData[1]; }
    [[nodiscard]] uint8_t& manuDataL() const override { return userEepromData[2]; }
    [[nodiscard]] uint8_t& manufacturerH() const override { return userEepromData[3]; }
    [[nodiscard]] uint8_t& manufacturerL() const override { return userEepromData[4]; }
    [[nodiscard]] uint8_t& deviceTypeH() const override { return userEepromData[5]; }
    [[nodiscard]] uint8_t& deviceTypeL() const override { return userEepromData[6]; }
    [[nodiscard]] uint8_t& version() const override { return userEepromData[7]; }
    [[nodiscard]] uint8_t& checkLimit() const override { return userEepromData[8]; }
    [[nodiscard]] uint8_t& appPeiType() const override { return userEepromData[9]; }
    [[nodiscard]] uint8_t& syncRate() const override { return userEepromData[10]; }
    [[nodiscard]] uint8_t& portCDDR() const override { return userEepromData[11]; }
    [[nodiscard]] uint8_t& portADDR() const override { return userEepromData[12]; }
    [[nodiscard]] uint8_t& runError() const override { return userEepromData[13]; }
    [[nodiscard]] uint8_t& routeCnt() const override { return userEepromData[14]; }
    [[nodiscard]] uint8_t& maxRetransmit() const override { return userEepromData[15]; }
    [[nodiscard]] uint8_t& confDesc() const override { return userEepromData[16]; }
    [[nodiscard]] uint8_t& assocTabPtr() const override { return userEepromData[17]; }
    [[nodiscard]] uint8_t& commsTabPtr() const override { return userEepromData[18]; }
    [[nodiscard]] uint8_t& usrInitPtr() const override { return userEepromData[19]; }
    [[nodiscard]] uint8_t& usrProgPtr() const override { return userEepromData[20]; }

    [[nodiscard]] uint8_t& addrTabSize() const override { return userEepromData[21]; }
    [[nodiscard]] uint8_t* addrTab() const override { return &userEepromData[22]; }
};



#endif /* TEST_SBLIB_KNX_USEREEPROM_H_ */
