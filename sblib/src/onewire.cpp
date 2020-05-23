
/*
 *  onewire.cpp - Device communications bus system  (by Dallas Semiconductor Corp.)
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/core.h>
#include <sblib/digital_pin.h>

#include <sblib/onewire.h>

/*****************************************************************************
** Function name:  OneWireInit
**
** Descriptions:   Initialize the OneWire.
**
** parameters:     PORT, bParasitePowerMode (default: false)
**
** Returned value: none
**
*****************************************************************************/
void OneWire::OneWireInit(int pin, bool bParasitePowerMode /* false */)
{
  this->_pin = pin;
  this->m_bParasitePowerMode= bParasitePowerMode;
  pinMode(this->_pin, INPUT |PULL_UP);
#if ONEWIRE_SEARCH
  this->OneWireResetSearch();
#endif
}

/*****************************************************************************
** Function name:  OneWireReset
**
** Descriptions:   Perform the 1-Wire reset cycle. Wwait up to 250uS for the
**                 bus to come high. If the bus does not come high, it is
**                 broken or shorted.
**
** parameters:     none
**
** Returned value: Returns true if a device responds with a presence pulse.
**                 Returns false if there is no device or the bus is shorted
**                 or otherwise held low for more than 250uS
**
*****************************************************************************/
bool OneWire::OneWireReset()
{
  bool bRet= false;
  uint8_t retries= 125;

  pinDisableInterrupt(this->_pin);
    // note if the bus was low to start with
    bool bHigh = digitalRead(this->_pin);
    pinMode(this->_pin, INPUT |PULL_UP);
#if ONEWIRE_INTERNAL_PULLUP
    digitalWrite(this->_pin, 1);       // enable pull-up resistor
#endif
    do {                               // wait until the wire is high... just in case
      if (--retries == 0) return 0;
      delayMicroseconds(2);
    } while ( !digitalRead(this->_pin));
    // stay high long enough for the chip to get it in case bus was low
    if (!bHigh) delayMicroseconds(60);
    pinMode(this->_pin,OUTPUT);         // drive output low
    digitalWrite(this->_pin, 0);
    delayMicroseconds(480);
    pinMode(this->_pin, INPUT |PULL_UP);// allow it to float
#if ONEWIRE_INTERNAL_PULLUP
    digitalWrite(this->_pin, 1);       // enable pull-up resistor
#endif
    delayMicroseconds(70);
    bRet= !digitalRead(this->_pin);
    delayMicroseconds(410);
  pinEnableInterrupt(this->_pin);
  return bRet;
}

/*****************************************************************************
** Function name:  OneWireWriteBit
**
** Descriptions:   Write a bit. Port and bit is used to cut lookup time and
**                 provide more certain timing. The bus is always left powered
**                 at the end, see note in OneWireWrite() about that.
**
** parameters:     Value to write
**
** Returned value: none
**
*****************************************************************************/
void OneWire::OneWireWriteBit(uint8_t uValue)
{
  pinDisableInterrupt(this->_pin);
  if(uValue & 1) {
    pinMode(this->_pin, OUTPUT);       // drive output low
    digitalWrite(this->_pin, 0);
    delayMicroseconds(10);
    digitalWrite(this->_pin, 1);       // drive output high
    delayMicroseconds(55);
  } else {
    pinMode(this->_pin, OUTPUT);       // drive output low
    digitalWrite(this->_pin, 0);
    delayMicroseconds(65);
    digitalWrite(this->_pin, 1);       // drive output high
    delayMicroseconds(5);
  }
  pinEnableInterrupt(this->_pin);
}

/*****************************************************************************
** Function name:  OneWireReadBit
**
** Descriptions:   Read a bit. Port and bit is used to cut lookup time and
**                 provide more certain timing.
**
** parameters:     none
**
** Returned value: bit
**
*****************************************************************************/
uint8_t OneWire::OneWireReadBit()
{
  uint8_t uRet;

  pinDisableInterrupt(this->_pin);
    pinMode(this->_pin, OUTPUT);
    digitalWrite(this->_pin, 0);
    delayMicroseconds(3);
    pinMode(this->_pin, INPUT |PULL_UP);// let pin float, pull up will raise
#if ONEWIRE_INTERNAL_PULLUP
    digitalWrite(this->_pin, 1);        // enable pull-up resistor
#endif
    delayMicroseconds(10);
    uRet = digitalRead(this->_pin);
    delayMicroseconds(53);
  pinEnableInterrupt(this->_pin);
  return uRet;
}

/*****************************************************************************
** Function name:  OneWireWrite
**
** Descriptions:   Write a byte. The writing code uses the active drivers to
**                 raise the pin high, if you need power after the write
**                 (e.g. DS18S20 in parasite power mode) then set
**                 'm_bParasitePowerMode' to 1, otherwise the pin will go
**                 tri-state at the end of the write to avoid heating in a short
**                 or other mishap.If 'm_bParasitePowerMode' is true, then the
**                 wire is held high at the end for parasitically powered
**                 devices. You are responsible for eventually depowering it by
**                 calling OneWireDePower() or doing another read or write.
**
** parameters:     Value
**
** Returned value: none
**
*****************************************************************************/
void OneWire::OneWireWrite(uint8_t v)
{
  uint8_t bitMask;
  for( bitMask = 0x01; bitMask; bitMask <<= 1)
  {
      this->OneWireWriteBit( (bitMask & v)?1:0);
  }

  if(!this->m_bParasitePowerMode)
  {
    pinDisableInterrupt(this->_pin);
      pinMode(this->_pin, INPUT);
#if !(ONEWIRE_INTERNAL_PULLUP)
      digitalWrite(this->_pin, 0);     // otherwise it's left high
#endif
   pinEnableInterrupt(this->_pin);
  }
}

/*****************************************************************************
** Function name:  OneWireWriteBytes
**
** Descriptions:   Write bytes. The writing code uses the active drivers to
**                 raise the pin high, if you need power after the write
**                 (e.g. DS18S20 in parasite power mode) then set
**                 'm_bParasitePowerMode' to 1, otherwise the pin will go tri-state
**                 at the end of the write to avoid heating in a short or other mishap.
**
** parameters:     Value buffer, Counter
**
** Returned value: none
**
*****************************************************************************/
void OneWire::OneWireWriteBytes(const uint8_t *buf, uint16_t count)
{
  for(uint16_t i = 0 ; i < count ; i++) this->OneWireWrite(buf[i]);
  if(!this->m_bParasitePowerMode)
  {
    pinDisableInterrupt(this->_pin);
      pinMode(this->_pin, INPUT);
#if !ONEWIRE_INTERNAL_PULLUP
      digitalWrite(_pin, 0);
#endif
    pinEnableInterrupt(this->_pin);
  }
}

/*****************************************************************************
** Function name:  OneWireRead
**
** Descriptions:   read a byte.
**
** parameters:     none
**
** Returned value: read byte
**
*****************************************************************************/
uint8_t OneWire::OneWireRead()
{
  uint8_t bitMask;
  uint8_t rRet = 0;
  for(bitMask = 0x01; bitMask; bitMask <<= 1)
  {
    if( this->OneWireReadBit()) rRet |= bitMask;
  }
  return rRet;
}

/*****************************************************************************
** Function name:  OneWireReadBytes
**
** Descriptions:   read bytes.
**
** parameters:     bytes buffer, read count
**
** Returned value: bytes buffer
**
*****************************************************************************/
void OneWire::OneWireReadBytes(uint8_t *buf, uint16_t count)
{
  for (uint16_t i = 0 ; i < count ; i++)
  {
    buf[i] = this->OneWireRead();
  }
}

/*****************************************************************************
** Function name:  OneWireSelect
**
** Descriptions:   Do a ROM select. Issue a 1-Wire rom select command, you do
**                 the reset first.
**
** parameters:     ROM
**
** Returned value: none
**
*****************************************************************************/
void OneWire::OneWireSelect(const uint8_t rom[8])
{
  this->OneWireWrite(0x55);            // Choose ROM
  for( uint8_t i = 0; i < 8; i++)
  {
    this->OneWireWrite(rom[i]);
  }
}

/*****************************************************************************
** Function name:  OneWireSkip
**
** Descriptions:   Do a ROM skip. Issue a 1-Wire rom skip command,
**                 to address all on bus.
**
** parameters:     none
**
** Returned value: none
**
*****************************************************************************/
void OneWire::OneWireSkip()
{
  this->OneWireWrite(0xCC);            // Skip ROM
}

/*****************************************************************************
** Function name:  OneWireDePower
**
** Descriptions:   Stop forcing power onto the bus. You only need to do this if
**                 you used the 'm_bParasitePowerMode' flag to OneWireWrite() or
**                 used a OneWireWriteBit() call and aren't about to do another
**                 read or write. You would rather not leave this powered if you
**                 don't have to, just in case someone shorts your bus.
**
** parameters:     none
**
** Returned value: none
**
*****************************************************************************/
void OneWire::OneWireDePower()
{
  pinDisableInterrupt(this->_pin);
    pinMode(this->_pin, INPUT);
    digitalWrite(this->_pin, 0);       // disable pull-up too
  pinEnableInterrupt(this->_pin);
}

#if ONEWIRE_SEARCH

/*****************************************************************************
** Function name:  OneWireResetSearch
**
** Descriptions:   You need to use this function to start a search again from
**                 the beginning. You do not need to do it for the first
**                 search, though you could. Clear the search state so that
**                 if will start from the beginning again.
**
** parameters:     none
**
** Returned value: none
**
*****************************************************************************/
void OneWire::OneWireResetSearch()
{
  this->_LastDiscrepancy= 0;           // reset the search state
  this->_bLastDeviceFlag= false;
  this->_LastFamilyDiscrepancy= 0;
  for(uint8_t i = 7; ; i--)
  {
    this->ROM_NO[i] = 0;
    if(i == 0) break;
  }
}

/*****************************************************************************
** Function name:  OneWireTargetSearch
**
** Descriptions:   Setup the search to find the device type 'family_code' on
**                 the next call to search(*newAddr) if it is present.
**
** parameters:     family_code (i.e. 0x10 for DS18S20 or 0x28 for DS18B20)
**
** Returned value: none
**
*****************************************************************************/
void OneWire::OneWireTargetSearch(uint8_t family_code)
{
  this->ROM_NO[0]= family_code;        // set the search state to find SearchFamily type devices
  for(uint8_t i = 1; i < 8; i++)
  {
    this->ROM_NO[i]= 0;
  }
  this->_LastDiscrepancy= 64;
  this->_LastFamilyDiscrepancy= 0;
  this->_bLastDeviceFlag= false;
}

/*****************************************************************************
** Function name:  OneWireSearch
**
** Descriptions:   Perform a search. If this function returns a '1' then it has
**                 enumerated the next device and you may retrieve the ROM from
**                 the OneWire::address variable. If there are no devices, no
**                 further devices, or something horrible happens in the middle
**                 of the enumeration then a 0 is returned.  If a new device is
**                 found then its address is copied to newAddr.
**                 Use OneWire::OneWireResetSearch() to start over.
**                 Perform the 1-Wire Search Algorithm on the 1-Wire bus using
**                 the existing search state.
**
**                 Look for the next device. Returns true if a new address has
**                 been returned. A false might mean that the bus is shorted,
**                 there are no devices, or you have already retrieved all of
**                 them. It  might be a good idea to check the CRC to make sure
**                 you didn't get garbage. The order is deterministic. You will
**                 always get the same devices in the same order.
**
** parameters:     family_code (i.e. 0x10 for DS18S20 or 0x28 for DS18B20)
**
** Returned value: true  --> device found, ROM number in ROM_NO buffer
**                 false --> device not found, end of search
**
*****************************************************************************/
bool OneWire::OneWireSearch(uint8_t *newAddr)
{
  bool bRet= false;
  // if the last call was not the last one
  if(this->_bLastDeviceFlag) return bRet;

  if(!this->OneWireReset())                              // 1-Wire reset
  {
    this->_LastDiscrepancy= 0;                           // reset the search
    this->_bLastDeviceFlag= false;
    this->_LastFamilyDiscrepancy= 0;
    return bRet;
  }

  // initialize for search
  uint8_t id_bit_number= 1, rom_byte_mask=1, last_zero=0, rom_byte_number=0;
  uint8_t search_direction, id_bit, cmp_id_bit;

  this->OneWireWrite(0xF0);                            // issue the search command

  do {                                                 // loop to do the search
    id_bit= this->OneWireReadBit();                    // read a bit and its complement
    cmp_id_bit= this->OneWireReadBit();
    if((id_bit == 1) && (cmp_id_bit == 1)) break;      // check for no devices on 1-wire
    else
    {
      if(id_bit != cmp_id_bit)                         // all devices coupled have 0 or 1
      {
        search_direction= id_bit;                      // bit write value for search
      }
      else
      {                                                // if this discrepancy if before the Last Discrepancy
        if(id_bit_number < this->_LastDiscrepancy) {   // on a previous next then pick the same as last time
          search_direction= ((this->ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
        } else search_direction= (id_bit_number == this->_LastDiscrepancy); // if equal to last pick 1, if not then pick 0

        if (search_direction == 0)                     // if 0 was picked then record its position in LastZero
        {
          last_zero= id_bit_number;
          if (last_zero < 9) this->_LastFamilyDiscrepancy= last_zero; // check for Last discrepancy in family
        }
      }
      if(search_direction == 1) this->ROM_NO[rom_byte_number] |= rom_byte_mask;
      else this->ROM_NO[rom_byte_number] &= ~rom_byte_mask;  // set or clear the bit in the ROM byte rom_byte_number with mask rom_byte_mask

      this->OneWireWriteBit(search_direction);         // serial number search direction write bit

      id_bit_number++;                                 // increment the byte counter id_bit_number and shift the mask rom_byte_mask
      rom_byte_mask <<= 1;

      if(rom_byte_mask == 0)                           // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
      {
        rom_byte_number++;
        rom_byte_mask = 1;
      }
    }
  } while(rom_byte_number < 8);                        // loop until through all ROM bytes 0-7

  if(!(id_bit_number < 65))                            // if the search was successful then
  {
    this->_LastDiscrepancy= last_zero;                 // search successful so set _LastDiscrepancy,_bLastDeviceFlag,search_result
    if(this->_LastDiscrepancy == 0) this->_bLastDeviceFlag= true; // check for last device
    bRet= true;
  }

  if(!bRet || !this->ROM_NO[0])                          // if no device found then reset counters so next 'search' will be like a first
  {
    this->_LastDiscrepancy= 0;
    this->_bLastDeviceFlag= false;
    this->_LastFamilyDiscrepancy= 0;
  } else
  {
    for (uint8_t i = 0; i < 8; i++) newAddr[i] = this->ROM_NO[i];
  }

  return bRet;
}
#endif // #if ONEWIRE_SEARCH

#if ONEWIRE_CRC
/*****************************************************************************
** Function name:  OneWireCRC8
**
** Descriptions:   Compute a Dallas Semiconductor 8 bit CRC directly. this is
**                 much slower, but much smaller, than a lookup table.
**
** parameters:     addr and len
**
** Returned value: CRC 8
**
*****************************************************************************/
uint8_t OneWire::OneWireCRC8(const uint8_t *addr, uint8_t len)
{
  uint8_t crc = 0;

  while(len--)
  {
    uint8_t inbyte = *addr++;
    for(uint8_t i = 8; i; i--)
    {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}

#if ONEWIRE_CRC16
/*****************************************************************************
** Function name:  OneWireCheckCRC16
**
** Descriptions:   Compute the 1-Wire CRC16 and compare it against the received CRC.

** parameters: [in] input - Array of bytes to checksum.
**             [in] len - How many bytes to use.
**             [in] inverted_crc - The two CRC16 bytes in the received data.
**                                 This should just point into the received
**                                 data, *not* at a 16-bit integer.
**             [in-optional] crc - The crc starting value (optional)
**
** Returned value: true, if the CRC matches.
**
*****************************************************************************/
bool OneWire::OneWireCheckCRC16(const uint8_t* input, uint16_t len, const uint8_t* inverted_crc, uint16_t crc)
{
  crc= ~OneWireCRC16(input, len, crc);
  return (crc & 0xFF) == inverted_crc[0] && (crc >> 8) == inverted_crc[1];
}

/*****************************************************************************
** Function name:  OneWireCRC16
**
** Descriptions:   Compute a Dallas Semiconductor 16 bit CRC.  This is required
**                 to check the integrity of data received from many 1-Wire
**                 devices.  Note that the CRC computed here is *not* what you'll
**                 get from the 1-Wire network, for two reasons:
**                 1) The CRC is transmitted bitwise inverted.
**                 2) Depending on the endian-ness of your processor, the binary
**                    representation of the two-byte return value may have a
**                    differentbyte order than the two bytes you get from 1-Wire.
**
** parameters:  [in] input - Array of bytes to checksum.
**              [in] len - How many bytes to use.
**              [in] crc - The crc starting value (optional)
**
**
** Returned value: The CRC16, as defined by Dallas Semiconductor.
**
*****************************************************************************/
uint16_t OneWire::OneWireCRC16(const uint8_t* input, uint16_t len, uint16_t crc)
{
  static const uint8_t oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
  for(uint16_t i = 0 ; i < len ; i++)
  {
    uint16_t cdata = input[i];         // Even though we're just copying a byte from the input,
    cdata = (cdata ^ crc) & 0xff;      // we'll be doing 16-bit computation with it.
    crc >>= 8;

    if(oddparity[cdata & 0x0F] ^ oddparity[cdata >> 4])
    {
      crc ^= 0xC001;
    }

    cdata <<= 6;
    crc ^= cdata;
    cdata <<= 1;
    crc ^= cdata;
  }
  return crc;
}
#endif // #if ONEWIRE_CRC16
#endif // #if ONEWIRE_CRC
