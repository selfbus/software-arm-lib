/*
 *  i2c.h - Inter-Integrated Circuit (I²C).
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_i2c_h
#define sblib_i2c_h

#include <sblib/platform.h>

/*
 * These are states returned by the I2CEngine:
 *
 * IDLE     - is never returned but only used internally
 * PENDING  - is never returned but only used internally in the I2C functions
 * ACK      - The transaction finished and the slave returned ACK (on all bytes)
 * NACK     - The transaction is aborted since the slave returned a NACK
 * SLA_NACK - The transaction is aborted since the slave returned a NACK on the SLA
 *            this can be intentional (e.g. an 24LC08 EEPROM states it is busy)
 *            or the slave is not available/accessible at all.
 * ARB_LOSS - Arbitration loss during any part of the transaction.
 *            This could only happen in a multi master system or could also
 *            identify a hardware problem in the system.
 */
#define I2CSTATE_IDLE     0x000
#define I2CSTATE_PENDING  0x001
#define I2CSTATE_ACK      0x101
#define I2CSTATE_NACK     0x102
#define I2CSTATE_SLA_NACK 0x103
#define I2CSTATE_ARB_LOSS 0x104

#define I2CSCAN_FOUND      0
#define I2CSCAN_ERROR      1
#define I2CSCAN_NOT_FOUND  2

#define MAX_SCAN_DEVICES   4

#define FAST_MODE_PLUS  0

#define BUFSIZE         8

#define MAX_TIMEOUT     0x00FFFFFF

#define I2CMASTER       0x01
#define I2CSLAVE        0x02

#define PCF8594_ADDR    0xA0
#define READ_WRITE      0x01

#define RD_BIT          0x01

#define I2CONSET_I2EN   0x00000040  // I2C Control Set Register
#define I2CONSET_AA     0x00000004
#define I2CONSET_SI     0x00000008
#define I2CONSET_STO    0x00000010
#define I2CONSET_STA    0x00000020

#define I2CONCLR_AAC    0x00000004  // I2C Control clear Register
#define I2CONCLR_SIC    0x00000008
#define I2CONCLR_STAC   0x00000020
#define I2CONCLR_I2ENC  0x00000040

#define I2DAT_I2C       0x00000000  // I2C Data Reg
#define I2ADR_I2C       0x00000000  // I2C Slave Address Reg
#define I2SCLH_SCLH     58          // I2C SCL Duty Cycle High Reg
#define I2SCLL_SCLL     57          // I2C SCL Duty Cycle Low Reg
#define I2SCLH_HS_SCLH  0x00000020  // Fast Plus I2C SCL Duty Cycle High Reg
#define I2SCLL_HS_SCLL  0x00000020  // Fast Plus I2C SCL Duty Cycle Low Reg

/****************************************************************************
* I2C Class
*****************************************************************************/
class I2C
{
public:
  static I2C* Instance();
  uint8_t I2CMasterBuffer[BUFSIZE];
  uint8_t I2CSlaveBuffer[BUFSIZE];

  uint16_t I2CMasterState;
  uint16_t I2CSlaveState;
  uint16_t I2CReadLength;
  uint16_t I2CWriteLength;
  uint16_t RdIndex;
  uint16_t WrIndex;

  uint8_t I2CScan_State;
  uint8_t I2CScan_uAdress[MAX_SCAN_DEVICES];

  bool bI2CIsInitialized;

  void I2CInit( uint8_t I2cMode=I2CMASTER );
  uint16_t I2CEngine( void );
  bool I2CStart();
  bool I2CStop();
  uint8_t I2CScan(uint8_t StartAdrr=0, uint8_t EndAdrr = 255);
  void ClearBuffer(bool bClearMaster=true, bool bClearSlave=true);
  bool Write(uint8_t uDeviceAddr, const char *udata, uint8_t data_length);
  bool Read(uint8_t uDeviceAddr, char *udata, uint8_t data_read_length, uint16_t uDelayMS=200);
private:
  I2C(); I2C(I2C const&);
  static I2C* m_pInstance;
};

#endif /* end __I2C_H */
