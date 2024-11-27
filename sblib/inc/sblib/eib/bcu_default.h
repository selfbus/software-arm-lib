/*
 *  bcu_default.h - BCU specific stuff.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  last update 2024 Horst Rauch
 *       added bus voltage monitoring for
 *       - start up phase in 2 steps: 21V > bus voltage >18V and Bus voltage >21V
 *       - normal operation: bus voltage is >=20V
 *       - power down in 2 phase: 18V< bus voltage is <20V Bus failure -> inform APP, stop KNX TX/RX
 *       - if Bus voltage < (18V or) SPMS-min++-Voltage stop all SW and put in reset state
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_BcuDefault_h
#define sblib_BcuDefault_h

#include <sblib/types.h>
#include <sblib/utils.h>
#include <sblib/mem_mapper.h>
#include <sblib/usr_callback.h>
#include <sblib/eib/userEeprom.h>
#include <sblib/timer.h>
#include <sblib/eib/bcu_base.h>
#include <sblib/eib/bus_voltage.h>


class Bus;

/**
 * Class for controlling all BCU related things.
 *
 * In order to use the EIB bus, you need to call bcu.begin() once in your application's
 * setup() function.
 */
class BcuDefault: public BcuBase
{
public:
    BcuDefault(UserRam* userRam, UserEeprom* userEeprom, ComObjects* comObjects, AddrTables* addrTables);
    BcuDefault() = delete;
    ~BcuDefault() = default;

    /**
     * End using the EIB bus coupling unit.
     */
    void end();

    /**
     * Set our own physical address. Normally the physical address is set by ETS when
     * programming the device.
     *
     * @param addr - the physical address
     */
    void setOwnAddress(uint16_t addr) override;

    /**
     * Get the mask version.
     * Usually 0x0012 for BCU1, 0x0020 for BCU2.
     */
    virtual uint16_t getMaskVersion() const = 0;

    virtual const char* getBcuType() const = 0;

    /**
     * The BCU's main processing loop. This is like the application's loop() function,
     * and is called automatically by main() when the BCU is activated with bcu.begin().
     *
     * We do the bus voltage monitoring here and trigger respective actions if necessary
     *
     */
    virtual void loop() override;
    
    /**
     * Process a APCI_MEMORY_WRITE_PDU
     * see KNX Spec. 3/3/7 �3.5.4 p.73 A_Memory_Write-service
     *
     * @param addressStart - memory start address
     * @param payLoad - data to write into the memory
     * @param lengthPayLoad - length of data/payload
     *
     * @return true if successfully written, otherwise false
     */
    virtual bool processApciMemoryWritePDU(int addressStart, byte *payLoad, int lengthPayLoad);

    /**
     * Process a APCI_MEMORY_READ_PDU
     * see KNX Spec. 3/3/7 �3.5.3 p.71 A_Memory_Read-service
     *
     * @param addressStart - memory start address
     * @param payLoad - buffer to write data read from memory
     * @param lengthPayLoad - length of data/payload to read
     *
     * @return true if successfully read, otherwise false
     */
    virtual bool processApciMemoryReadPDU(int addressStart, byte *payLoad, int lengthPayLoad);

    /**
     * Process a APCI_MEMORY_READ_PDU or APCI_MEMORY_WRITE_PDU depending on
     *
     * @param addressStart - memory start address
     * @param payLoad - data to write into the memory
     * @param lengthPayLoad - length of data/payload
     * @param readMem - operation mode, if true memory will be read, if false memory will be written
     *
     * @return true if successfully, otherwise false
     */
    bool processApciMemoryOperation(unsigned int addressStart, byte *payLoad, unsigned int lengthPayLoad, const bool &readMem);

    /**
     * Process a APCI_MASTER_RESET_PDU
     * see KNX Spec. 3/5/2 §3.7.1.2 p.64 A_Restart
     *
     * @param sendBuffer    Buffer to write the (potential) response into
     * @param eraseCode     eraseCode of the @ref APCI_MASTER_RESET_PDU telegram
     * @param channelNumber channelNumber of the @ref APCI_MASTER_RESET_PDU telegram
     *
     * @return true if a response message should be sent, otherwise false
     */
    bool processApciMasterResetPDU(uint8_t * sendBuffer, uint8_t eraseCode, uint8_t channelNumber);

    /**
     * Processes @ref APCI_ADC_READ_PDU,
     *           @ref APCI_MEMORY_READ_PDU,
     *           @ref APCI_MEMORY_WRITE_PDU,
     *           @ref APCI_DEVICEDESCRIPTOR_READ_PDU,
     *           @ref APCI_MASTER_RESET_PDU,
     *           @ref APCI_AUTHORIZE_REQUEST_PDU telegrams.
     *
     * For all other telegrams @ref BcuBase::processApci is invoked.
     *
     * @param apciCmd       @ref ApciCommand of the telegram
     * @param telegram      The APCI-telegram
     * @param telLength     Telegram length
     * @param sendBuffer    Pointer to the buffer for a potential response telegram
     * @return True if a response telegram was prepared, otherwise false
     */
    virtual bool processApci(ApciCommand apciCmd, unsigned char * telegram, uint8_t telLength, uint8_t * sendBuffer);

    /**
      * @brief Performs a system reset by calling @ref NVIC_SystemReset
      * @details Before the reset a USR_CALLBACK_RESET is send to the application,
      *          the UserEprom and memMapper are written to flash.
      * @warning This function will never return.
      */
    void softSystemReset() override;

    UserEeprom* userEeprom;

    byte* userMemoryPtr(unsigned int addr);

    /**
     * Returns a pointer to the instance of the MemMapper object of the BCU
     * @return a pointer to the instance of the MemMapper object, in case of error return is nullptr
     */
    MemMapper* getMemMapper();

    /**
     * Allow an user provided memory mapper to store parameter data via memory write / read
     * @param mapper - a pointer to an instance of a MemMapper object
     */
    void setMemMapper(MemMapper *mapper);



    /**
      * Set a callback class to notify the user program of some events
      */
     void setUsrCallback(UsrCallback *callback);

    /**
      * Set a callback class to get the user supplied adc count value
      */
    void setADCCallback(ADCCallback *bcu_adcCallback);

    /**
     * Enable/Disable sending of group write or group response telegrams.
     * Useful if the device wants to implement transmission delays
     * after bus voltage recovery.
     * Transmission is enabled by default.
     */
    void enableGroupTelSend(bool enable);

    /**
     * Set a limit for group telegram transmissions per second.
     * If the parameter is not zero, there is a minimum delay
     * of 1/limit (in seconds) between subsequent group telegram
     * transmissions.
     *
     * @param limit - the maximum number of telegrams per second.
     */
    void setGroupTelRateLimit(unsigned int limit);

protected:
    /*
     * Special initialization for the BCU
     * e.g. Physical Layer init, Data Layer init,
     * TLayer init,...
     */
    virtual void _begin() override;

    /**
     * Begin using the EIB bus coupling unit, and set the manufacturer-ID, device type, program version
     * KNX phy address, BCU data in usrRam and usrEprom
     *
     * @param manufacturer - the manufacturer ID (16 bit)
     * @param deviceType - the device type (16 bit)
     * @param version - the version of the application program (8 bit)
     */
    virtual void begin(int manufacturer, int deviceType, int version);

    /**
     * Process a group address (T_Data_Group) telegram.
     */
    virtual bool processGroupAddressTelegram(ApciCommand apciCmd, uint16_t groupAddress, unsigned char *telegram, uint8_t telLength) override;

    /**
     * Process a broadcast telegram.
     */
    virtual bool processBroadCastTelegram(ApciCommand apciCmd, unsigned char *telegram, uint8_t telLength) override;

    /**
     * Process a device-descriptor-read request.
     *
     * @param sendBuffer - Buffer to write the response into
     * @param id - the device-descriptor type ID
     *
     * @return True on success, false on failure
     */
    bool processDeviceDescriptorReadTelegram(uint8_t * sendBuffer, int id);

    /**
     * Flushes all pending write operations to user memory (eeprom, memmapper) and sends a callback to the user
     *
     * @param reason    The reason, why the memory is flushed
     * @return  True if eeprom was flushed, otherwise false
     */
    bool flushUserMemory(UsrCallbackType reason);

    MemMapper *memMapper;
    UsrCallback *usrCallback;
    ADCCallback *bcu_adcCallback;
    bool sendGrpTelEnabled;        //!< Sending of group telegrams is enabled. Usually set, but can be disabled.
    unsigned int groupTelWaitMillis;
    unsigned int groupTelSent;
    // for any ADC read from bus or app the BCU has only the bus voltage channel implemented


private:
};


#define  THRESHOLD_VOLTAGE_FAILED_mV 20000
#define  THRESHOLD_VOLTAGE_RETURN_mV 21000
#define  THRESHOLD_VOLTAGE_IDLE_mV 18000
#define  MAX_GROUP_TEL_PER_SECOND  28
#define  DEFAULT_GROUP_TEL_WAIT_MILLIS  1000/MAX_GROUP_TEL_PER_SECOND

#endif /*sblib_BcuDefault_h*/
