/*
 *  i2c.cpp - Inter-Integrated Circuit (I²C).
 *
 *  Copyright (c) 2015 Erkan Colak <erkanc@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/core.h>
#include <sblib/interrupt.h>

#include <sblib/i2c.h>

static I2C * i2c_m_pInstance;
I2C* I2C::m_pInstance = 0;

/*****************************************************************************
** Function name:  I2C
**
** Descriptions:   Constructor I2C
**
** parameters:     None
** Returned value: None
**
*****************************************************************************/
I2C::I2C()
{
  i2c_m_pInstance= this;
/*  RdIndex = 0;
  WrIndex = 0;
  I2CScan_State= I2CSCAN_NOT_FOUND;

  // I2CScan_uAdress[MAX_SCAN_DEVICES]={0};
  for( uint8_t i = 0; i < MAX_SCAN_DEVICES; i++ ) { I2CScan_uAdress[i] = 0; }

  I2CMasterState = I2CSTATE_IDLE;
  I2CSlaveState = I2CSTATE_IDLE;*/
}

/*****************************************************************************
** Function name: I2CInit
**
** Descriptions:  Initialize I2C controller
**
** parameters:    I2c mode is either MASTER or SLAVE
** Returned value:  true or false, return false if the I2C
**                  interrupt handler was not installed correctly
**
*****************************************************************************/
void  I2C::I2CInit( uint8_t I2cMode )
{
  this->bI2CIsInitialized= true;
  this->RdIndex = 0;
  this->WrIndex = 0;
  this->I2CScan_State= I2CSCAN_NOT_FOUND;

  // I2CScan_uAdress[MAX_SCAN_DEVICES]={0};
  for( uint8_t i = 0; i < MAX_SCAN_DEVICES; i++ ) { this->I2CScan_uAdress[i] = 0; }

  this->I2CMasterState = I2CSTATE_IDLE;
  this->I2CSlaveState = I2CSTATE_IDLE;

  LPC_SYSCON->PRESETCTRL |= (0x1<<1);
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);

  LPC_IOCON->PIO0_4 &= ~0x3F;   /*  I2C I/O config */
  LPC_IOCON->PIO0_4 |= 0x01;    /* I2C SCL */
  LPC_IOCON->PIO0_5 &= ~0x3F;
  LPC_IOCON->PIO0_5 |= 0x01;    /* I2C SDA */

  // --- Clear flags ---
  LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;

  // --- Reset registers ---
#if FAST_MODE_PLUS
  LPC_IOCON->PIO0_4 |= (0x1<<9);
  LPC_IOCON->PIO0_5 |= (0x1<<9);
  LPC_I2C->SCLL   = I2SCLL_HS_SCLL;
  LPC_I2C->SCLH   = I2SCLH_HS_SCLH;
#else
  LPC_I2C->SCLL   = I2SCLL_SCLL;
  LPC_I2C->SCLH   = I2SCLH_SCLH;
#endif

  if ( I2cMode == I2CSLAVE )
  {
    LPC_I2C->ADR0 = PCF8594_ADDR;
  }

  // --- Enable the I2C Interrupt ---
  enableInterrupt(I2C_IRQn);

  LPC_I2C->CONSET = I2CONSET_I2EN;
}

/*****************************************************************************
** Function name:  Instance
**
** Descriptions:   Only allow one instance of I2c to be generated.
**
** parameters:     None
** Returned value: None
**
*****************************************************************************/
I2C* I2C::Instance()
{
   if (!m_pInstance)
      m_pInstance = new I2C;
   return m_pInstance;
}

/*****************************************************************************
** Function name:   I2C_IRQHandler
**
** Descriptions:    I2C interrupt handler, deal with master mode only.
**
** parameters:      None
** Returned value:  None
** 
*****************************************************************************/
extern "C" {
  volatile uint8_t StatValue = 0;
  void I2C_IRQHandler(void)
  {

    /* this handler deals with master read and master write only */
    StatValue = LPC_I2C->STAT;
    switch ( StatValue )
    {
    case 0x08:
      /*
       * A START condition has been transmitted.
       * We now send the slave address and initialize
       * the write buffer
       * (we always start with a write after START+SLA)
       */
      i2c_m_pInstance->WrIndex = 0;
      LPC_I2C->DAT = i2c_m_pInstance->I2CMasterBuffer[i2c_m_pInstance->WrIndex++];
      LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
      i2c_m_pInstance->I2CMasterState = I2CSTATE_PENDING;
      break;

    case 0x10:
      /*
       * A repeated START condition has been transmitted.
       * Now a second, read, transaction follows so we
       * initialize the read buffer.
       */
      i2c_m_pInstance->RdIndex = 0;
      /* Send SLA with R bit set, */
      LPC_I2C->DAT = i2c_m_pInstance->I2CMasterBuffer[i2c_m_pInstance->WrIndex++];
      LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
    break;

    case 0x18:
      /*
       * SLA+W has been transmitted; ACK has been received.
       * We now start writing bytes.
       */
      i2c_m_pInstance->I2CScan_State = I2CSCAN_FOUND;
      LPC_I2C->DAT = i2c_m_pInstance->I2CMasterBuffer[i2c_m_pInstance->WrIndex++];
      LPC_I2C->CONCLR = I2CONCLR_SIC;
      break;

    case 0x20:
      /*
       * SLA+W has been transmitted; NOT ACK has been received.
       * Send a stop condition to terminate the transaction
       * and signal I2CEngine the transaction is aborted.
       */
      LPC_I2C->CONSET = I2CONSET_STO;
      LPC_I2C->CONCLR = I2CONCLR_SIC;
      i2c_m_pInstance->I2CMasterState = I2CSTATE_SLA_NACK;
      break;

    case 0x28:
      /*
       * Data in I2DAT has been transmitted; ACK has been received.
       * Continue sending more bytes as long as there are bytes to send
       * and after this check if a read transaction should follow.
       */
      if ( i2c_m_pInstance->WrIndex < i2c_m_pInstance->I2CWriteLength )
      {
        /* Keep writing as long as bytes avail */
        LPC_I2C->DAT = i2c_m_pInstance->I2CMasterBuffer[i2c_m_pInstance->WrIndex++];
      }
      else
      {
        if ( i2c_m_pInstance->I2CReadLength != 0 )
        {
          /* Send a Repeated START to initialize a read transaction */
          /* (handled in state 0x10)                                */
          LPC_I2C->CONSET = I2CONSET_STA;  /* Set Repeated-start flag */
        }
        else
        {
          i2c_m_pInstance->I2CMasterState = I2CSTATE_ACK;
          LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
        }
      }
      LPC_I2C->CONCLR = I2CONCLR_SIC;
      break;

    case 0x30:
      /*
       * Data byte in I2DAT has been transmitted; NOT ACK has been received
       * Send a STOP condition to terminate the transaction and inform the
       * I2CEngine that the transaction failed.
       */
      LPC_I2C->CONSET = I2CONSET_STO;
      LPC_I2C->CONCLR = I2CONCLR_SIC;
      i2c_m_pInstance->I2CMasterState = I2CSTATE_NACK;
      break;

    case 0x38:
      /*
       * Arbitration loss in SLA+R/W or Data bytes.
       * This is a fatal condition, the transaction did not complete due
       * to external reasons (e.g. hardware system failure).
       * Inform the I2CEngine of this and cancel the transaction
       * (this is automatically done by the I2C hardware)
       */
      i2c_m_pInstance->I2CScan_State = I2CSCAN_ERROR;
      i2c_m_pInstance->I2CMasterState = I2CSTATE_ARB_LOSS;
      LPC_I2C->CONCLR = I2CONCLR_SIC;
      break;

    case 0x40:
      /*
       * SLA+R has been transmitted; ACK has been received.
       * Initialize a read.
       * Since a NOT ACK is sent after reading the last byte,
       * we need to prepare a NOT ACK in case we only read 1 byte.
       */
      if ( i2c_m_pInstance->I2CReadLength == 1 )
      {
        /* last (and only) byte: send a NACK after data is received */
        LPC_I2C->CONCLR = I2CONCLR_AAC;
      }
      else
      {
        /* more bytes to follow: send an ACK after data is received */
        LPC_I2C->CONSET = I2CONSET_AA;
      }
      LPC_I2C->CONCLR = I2CONCLR_SIC;
      break;

    case 0x48:
      /*
       * SLA+R has been transmitted; NOT ACK has been received.
       * Send a stop condition to terminate the transaction
       * and signal I2CEngine the transaction is aborted.
       */
      LPC_I2C->CONSET = I2CONSET_STO;
      LPC_I2C->CONCLR = I2CONCLR_SIC;
      i2c_m_pInstance->I2CMasterState = I2CSTATE_SLA_NACK;
      break;

    case 0x50:
      /*
       * Data byte has been received; ACK has been returned.
       * Read the byte and check for more bytes to read.
       * Send a NOT ACK after the last byte is received
       */
      i2c_m_pInstance->I2CSlaveBuffer[i2c_m_pInstance->RdIndex++] = LPC_I2C->DAT;
      if ( i2c_m_pInstance->RdIndex < (i2c_m_pInstance->I2CReadLength-1) )
      {
        /* lmore bytes to follow: send an ACK after data is received */
        LPC_I2C->CONSET = I2CONSET_AA;
      }
      else
      {
        /* last byte: send a NACK after data is received */
        LPC_I2C->CONCLR = I2CONCLR_AAC;
      }
      LPC_I2C->CONCLR = I2CONCLR_SIC;
      break;

    case 0x58:
      /*
       * Data byte has been received; NOT ACK has been returned.
       * This is the last byte to read.
       * Generate a STOP condition and flag the I2CEngine that the
       * transaction is finished.
       */
      i2c_m_pInstance->I2CSlaveBuffer[i2c_m_pInstance->RdIndex++] = LPC_I2C->DAT;
      i2c_m_pInstance->I2CMasterState = I2CSTATE_ACK;
      LPC_I2C->CONSET = I2CONSET_STO;  /* Set Stop flag */
      LPC_I2C->CONCLR = I2CONCLR_SIC;  /* Clear SI flag */
      break;


    default:
      LPC_I2C->CONCLR = I2CONCLR_SIC;
    break;
    }
  }
} // extern "C"

/*****************************************************************************
** Function name:  I2CStart
**
** Descriptions:  Create I2C start condition, a timeout value is set if the
**                I2C never gets started, and timed out. It's a fatal error.
**
** parameters:    None
** Returned value:  true or false, return false if timed out
** 
*****************************************************************************/
bool I2C::I2CStart( void )
{
  uint32_t timeout = 0;

  // --- Issue a start condition ---
  LPC_I2C->CONSET = I2CONSET_STA;  // Set Start flag
    
  while((this->I2CMasterState != I2CSTATE_PENDING) && (timeout < MAX_TIMEOUT))
  {
    timeout++;
  }

  return (timeout < MAX_TIMEOUT);
}

/*****************************************************************************
** Function name:  I2CStop
**
** Descriptions:  Set the I2C stop condition
**
** parameters:    None
** Returned value:  true or never return
** 
*****************************************************************************/
bool I2C::I2CStop( void )
{
  uint32_t timeout = 0;

  LPC_I2C->CONSET = I2CONSET_STO;   // Set Stop flag
  LPC_I2C->CONCLR = I2CONCLR_SIC;   // Clear SI flag

  //--- Wait for STOP detected ---
  while((LPC_I2C->CONSET & I2CONSET_STO) && (timeout < MAX_TIMEOUT))
  {
    timeout++;
  }
  return (timeout >= MAX_TIMEOUT);
}



/*****************************************************************************
** Function name:  I2CEngine
**
** Descriptions:  The routine to complete a I2C transaction from start to stop.
**                All the intermitten steps are handled in the interrupt handler.
**                Before this routine is called, the read length, write length
**                and I2C master buffer need to be filled.
**
** parameters:    None
** Returned value: Any of the I2CSTATE_... values. See i2c.h
** 
*****************************************************************************/
uint16_t I2C::I2CEngine( void )
{
  this->I2CMasterState = I2CSTATE_IDLE;
  this->RdIndex = 0;
  this->WrIndex = 0;
  if ( this->I2CStart() != true )
  {
    this->I2CStop();
    return ( 0 /*FALSE*/ );
  }

  // wait until the state is a terminal state
  while (this->I2CMasterState < 0x100);

  return ( this->I2CMasterState );
}

/*****************************************************************************
** Function name:  I2CScan
**
** Descriptions: Will scan the I2C bus to find I2C device. If a device is found,
**               the Device address will be stored in I2CScan_uAdress[x].
**               Where x is iFoundDevices-1
**               Valid slave addresses are from 0x00 to 0xFF(255)
**
** parameters:    Start Address
** Returned value: Number of devices (iFoundDevices).
**
*****************************************************************************/
uint8_t I2C::I2CScan(uint8_t StartAdrr, uint8_t EndAdrr)
{
  uint8_t iScanAddr, iFoundDevices=0;
  if(EndAdrr > 0xff ) EndAdrr = 0xff;
  if(StartAdrr > 0xff ) StartAdrr = 0xff;
  for( iScanAddr = StartAdrr; iScanAddr < EndAdrr && iFoundDevices < MAX_SCAN_DEVICES ; iScanAddr++ )
  {
    this->ClearBuffer(true, false); // clear MasterBuffer Only
      this->I2CScan_State      = I2CSCAN_NOT_FOUND;
      this->I2CWriteLength     = 1;
      this->I2CReadLength      = 0;
      this->I2CMasterBuffer[0] = iScanAddr;
    this->I2CEngine();

    if( this->I2CScan_State == I2CSCAN_FOUND ) {
      this->I2CScan_uAdress[iFoundDevices]= this->I2CMasterBuffer[0];
      iFoundDevices++;
    }
  }
  return iFoundDevices;
}

/*****************************************************************************
** Function name: ClearBuffer
**
** Descriptions: clear MasterBuffer and or SlaveBuffer
**
** parameters:    MasterBuffer, SlaveBuffer
** Returned value: none
**
*****************************************************************************/
void I2C::ClearBuffer(bool bClearMaster, bool bClearSlave){
  if(bClearMaster || bClearSlave) {
    for ( uint8_t i = 0; i < BUFSIZE; i++ ) // clear buffer
    {
      if(bClearMaster) this->I2CMasterBuffer[i] = 0;
      if(bClearSlave) this->I2CSlaveBuffer[i] = 0;
    }
  }
}

/*****************************************************************************
** Function name: Write
**
** Descriptions:
**
** parameters:
** Returned value:
**
*****************************************************************************/
bool I2C::Write(uint8_t uDeviceAddr, const char *udata, uint8_t data_length)
{
  this->ClearBuffer(true, false); // clear MasterBuffer Only
    this->I2CWriteLength     = 1 + data_length;
    this->I2CReadLength      = 0;
    this->I2CMasterBuffer[0] = uDeviceAddr;
    for(uint8_t i=0; i < data_length; i++)
    {
      this->I2CMasterBuffer[i+1] = udata[i];
    }
  return (this->I2CEngine() == I2CSTATE_ACK) ? true:false;
}

/*****************************************************************************
** Function name: Read
**
** Descriptions:
**
** parameters:
** Returned value:
**
*****************************************************************************/
bool I2C::Read(uint8_t uDeviceAddr, char *udata, uint8_t data_read_length, uint16_t uDelayMS)
{
  delay(uDelayMS);
  bool bRet=false;
  this->ClearBuffer(true, true); // clear MasterBuffer and SlaveBuffer
    this->I2CWriteLength     = 2;
    this->I2CReadLength      = data_read_length;
    this->I2CMasterBuffer[0] = uDeviceAddr;
    this->I2CMasterBuffer[1] = udata[0];    /* offset read address */
    this->I2CMasterBuffer[2] = uDeviceAddr | RD_BIT;
  if( this->I2CEngine() == I2CSTATE_ACK )
  {
    bRet= true;
    for( uint8_t i=0; i < data_read_length; i++ )
    {
      udata[i]= this->I2CSlaveBuffer[i];
    }
  }
  return bRet;
}
