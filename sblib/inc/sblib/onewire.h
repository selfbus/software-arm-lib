/*
 *  onewire.h - device communications bus system (by Dallas Semiconductor Corp.)
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as published
 *  by the Free Software Foundation. Based on: Copyright (c) 2008, Tom Pollard,
 *  pollard@alum.mit.edu, Copyright (c) 2007, Jim Studt, Josh Larios. Copyright
 *  (c) 2000 Dallas Semiconductor Corporation, All Rights Reserved. Version 2.3.
 *  Latest version of this library: https://github.com/PaulStoffregen/OneWire
*/

#ifndef onewire_h
#define onewire_h

/*
 *  Supported and tested 1-Wire Devices:
 *    - DS18S20, DS18B20, DS1822 Temperature Sensors
 *    - DS2408 8-Channel Addressable Switch
 *    - DS250x Add-only programmable memory reader w/SKIP ROM.
 */

#define ONEWIRE_SEARCH          1      // Exclude onewire_search by defining this to '0'
#define ONEWIRE_CRC             1      // Exclude CRC checks altogether by defining this to '0'
#define ONEWIRE_CRC16           1      // Allow 16-bit CRC checks by defining this to 1 (Note that ONEWIRE_CRC must also be 1.)
#define ONEWIRE_INTERNAL_PULLUP 1      // Use internal pull-up resistor instead of regular 4.7KOhms (Info -> http://wp.josh.com/2014/06/23/no-external-pull-up-needed-for-ds18b20-temp-sensor/)

/*
 * OneWire Class
 */
class OneWire
{
  private:
    int _pin;
#if ONEWIRE_SEARCH                     // Global search state
    byte ROM_NO[8];           // Device ROM. 64bit Lasered ROM-Code to detect the Family Code
    uint8_t _LastDiscrepancy;
    uint8_t _LastFamilyDiscrepancy;
    bool _bLastDeviceFlag;
#endif

  public:
    bool m_bParasitePowerMode;        // Parasite power mode state

   /*
    * Function name:  OneWireInit
    * Descriptions:   Initialize the OneWire
    * parameters:     PORT, bParasitePowerMode (default: false)
    * Returned value: none
    */
    void OneWireInit( int pin, bool bParasitePowerMode= false);

   /*
    * Function name:  OneWireReset
    * Descriptions:   Perform the 1-Wire reset cycle. Wwait up to 250uS for the
    *                 bus to come high. If the bus does not come high, it is
    *                 broken or shorted.
    * parameters:     none
    * Returned value: Returns true if a device responds with a presence pulse.
    *                 Returns false if there is no device or the bus is shorted
    *                 or otherwise held low for more than 250uS
    */
    bool OneWireReset();

   /*
    * Function name:  OneWireWriteBit
    * Descriptions:   Write a bit. Port and bit is used to cut lookup time and
    *                 provide more certain timing. The bus is always left powered
    *                 at the end, see note in OneWireWrite() about that.
    * parameters:     Value to write
    * Returned value: none
    */
    void OneWireWriteBit(uint8_t v);

   /*
    * Function name:  OneWireReadBit
    * Descriptions:   Read a bit. Port and bit is used to cut lookup time and
    *                 provide more certain timing.
    * parameters:     none
    * Returned value: bit
    */
    uint8_t OneWireReadBit(void);

   /*
    * Function name:  OneWireWrite
    * Descriptions:   Write a byte. The writing code uses the active drivers to
    *                 raise the pin high, if you need power after the write
    *                 (e.g. DS18S20 in parasite power mode) then set
    *                 'm_bParasitePowerMode' to 1, otherwise the pin will go
    *                 tri-state at the end of the write to avoid heating in a short
    *                 or other mishap.If 'm_bParasitePowerMode' is true, then the
    *                 wire is held high at the end for parasitically powered
    *                 devices. You are responsible for eventually depowering it by
    *                 calling OneWireDePower() or doing another read or write.
    * parameters:     Value
    * Returned value: none
    */
    void OneWireWrite(uint8_t v);

   /*
    * Function name:  OneWireWriteBytes
    * Descriptions:   Write bytes. The writing code uses the active drivers to
    *                 raise the pin high, if you need power after the write
    *                 (e.g. DS18S20 in parasite power mode) then set
    *                 'm_bParasitePowerMode' to 1, otherwise the pin will go tri-state
    *                 at the end of the write to avoid heating in a short or other mishap.
    * parameters:     Value buffer, Counter
    * Returned value: none
    */
    void OneWireWriteBytes(const uint8_t *buf, uint16_t count);

   /*
    * Function name:  OneWireRead
    * Descriptions:   read a byte.
    * parameters:     none
    * Returned value: read byte
    */
    uint8_t OneWireRead(void);

   /*
    * Function name:  OneWireReadBytes
    * Descriptions:   read bytes.
    * parameters:     bytes buffer, read count
    * Returned value: bytes buffer
    */
    void OneWireReadBytes(uint8_t *buf, uint16_t count);

   /*
    * Function name:  OneWireSelect
    * Descriptions:   Do a ROM select. Issue a 1-Wire rom select command, you do
    *                 the reset first.
    * parameters:     ROM
    * Returned value: none
    */
    void OneWireSelect(const uint8_t rom[8]);

   /*
    * Function name:  OneWireSkip
    * Descriptions:   Do a ROM skip. Issue a 1-Wire rom skip command,
    *                 to address all on bus.
    * parameters:     none
    * Returned value: none
    */
    void OneWireSkip(void);

   /*
    * Function name:  OneWireDePower
    * Descriptions:   Stop forcing power onto the bus. You only need to do this if
    *                 you used the 'm_bParasitePowerMode' flag to OneWireWrite() or
    *                 used a OneWireWriteBit() call and aren't about to do another
    *                 read or write. You would rather not leave this powered if you
    *                 don't have to, just in case someone shorts your bus.
    * parameters:     none
    * Returned value: none
    */
    void OneWireDePower(void);

    /*
     * Function name: IsParasiteMode
     * Description:   Current Parasite power mode state
     * parameters:    none
     * Returned value: the current power mode state
     */
    inline bool IsParasiteMode(){ return this->m_bParasitePowerMode; }

#if ONEWIRE_SEARCH
   /*
    * Function name:  OneWireResetSearch
    * Descriptions:   You need to use this function to start a search again from
    *                 the beginning. You do not need to do it for the first
    *                 search, though you could. Clear the search state so that
    *                 if will start from the beginning again.
    * parameters:     none
    * Returned value: none
    */
    void OneWireResetSearch();

   /*
    * Function name:  OneWireTargetSearch
    * Descriptions:   Setup the search to find the device type 'family_code' on
    *                 the next call to search(*newAddr) if it is present.
    * parameters:     family_code (i.e. 0x10 for DS18S20 or 0x28 for DS18B20)
    * Returned value: none
    */
    void OneWireTargetSearch(uint8_t family_code);

   /*
    * Function name:  OneWireSearch
    * Descriptions:   Perform a search. If this function returns a '1' then it has
    *                 enumerated the next device and you may retrieve the ROM from
    *                 the OneWire::address variable. If there are no devices, no
    *                 further devices, or something horrible happens in the middle
    *                 of the enumeration then a 0 is returned.  If a new device is
    *                 found then its address is copied to newAddr.
    *                 Use OneWire::OneWireResetSearch() to start over.
    *                 Perform the 1-Wire Search Algorithm on the 1-Wire bus using
    *                 the existing search state.
    *                 Look for the next device. Returns true if a new address has
    *                 been returned. A false might mean that the bus is shorted,
    *                 there are no devices, or you have already retrieved all of
    *                 them. It  might be a good idea to check the CRC to make sure
    *                 you didn't get garbage. The order is deterministic. You will
    *                 always get the same devices in the same order.
    * parameters:     family_code (i.e. 0x10 for DS18S20 or 0x28 for DS18B20)
    * Returned value: true  --> device found, ROM number in ROM_NO buffer
    *                 false --> device not found, end of search
    */
    bool OneWireSearch(uint8_t *newAddr);
#endif
#if ONEWIRE_CRC
   /*
    * Function name:  OneWireCRC8
    * Descriptions:   Compute a Dallas Semiconductor 8 bit CRC directly. this is
    *                 much slower, but much smaller, than a lookup table.
    * parameters:     addr and len
    * Returned value: CRC 8
    */
    static uint8_t OneWireCRC8(const uint8_t *addr, uint8_t len);
#if ONEWIRE_CRC16

   /*
    * Function name:  OneWireCheckCRC16
    * Descriptions:   Compute the 1-Wire CRC16 and compare it against the received CRC.
    * parameters: [in] input - Array of bytes to checksum.
    *             [in] len - How many bytes to use.
    *             [in] inverted_crc - The two CRC16 bytes in the received data.
    *                                 This should just point into the received
    *                                 data, *not* at a 16-bit integer.
    *             [in-optional] crc - The crc starting value (optional)
    * Returned value: true, if the CRC matches.
    */
    static bool OneWireCheckCRC16(const uint8_t* input, uint16_t len, const uint8_t* inverted_crc, uint16_t crc = 0);

   /*
    * Function name:  OneWireCRC16
    * Descriptions:   Compute a Dallas Semiconductor 16 bit CRC.  This is required
    *                 to check the integrity of data received from many 1-Wire
    *                 devices.  Note that the CRC computed here is *not* what you'll
    *                 get from the 1-Wire network, for two reasons:
    *                 1) The CRC is transmitted bitwise inverted.
    *                 2) Depending on the endian-ness of your processor, the binary
    *                    representation of the two-byte return value may have a
    *                    differentbyte order than the two bytes you get from 1-Wire.
    * parameters:  [in] input - Array of bytes to checksum.
    *              [in] len - How many bytes to use.
    *              [in] crc - The crc starting value (optional)
    * Returned value: The CRC16, as defined by Dallas Semiconductor.
    */
    static uint16_t OneWireCRC16(const uint8_t* input, uint16_t len, uint16_t crc = 0);
#endif
#endif
};

#endif /* 1wire_h */
