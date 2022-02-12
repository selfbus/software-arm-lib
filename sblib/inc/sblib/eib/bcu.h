/*
 *  bcu.h - BCU specific stuff.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_bcu_h
#define sblib_bcu_h

#include <sblib/eib/bcu_base.h>
#include <sblib/types.h>
#include <sblib/eib/bcu_type.h>
#include <sblib/eib/properties.h>
#include <sblib/eib/user_memory.h>
#include <sblib/utils.h>
#include <sblib/mem_mapper.h>
#include <sblib/usr_callback.h>


// Rename the method begin_BCU() of the class BCU to indicate the BCU type. If you get a
// link error then the library's BCU_TYPE is different from the application's BCU_TYPE.
#define begin_BCU  CPP_CONCAT_EXPAND(begin_,BCU_NAME)

/**
 * Class for controlling all BCU related things.
 *
 * In order to use the EIB bus, you need to call bcu.begin() once in your application's
 * setup() function.
 */
class BCU : public BcuBase
{
public:
    /**
     * @brief Process a group address (T_Data_Group) telegram.
     */
    virtual bool processGroupAddressTelegram(unsigned char *telegram, unsigned short telLength);

    /**
     * @brief Process a broadcast telegram.
     */
    virtual bool processBroadCastTelegram(unsigned char *telegram, unsigned short telLength);

    virtual unsigned char processApci(int apci, const int senderAddr, const int senderSeqNo, bool *sendResponse, unsigned char *telegram, unsigned short telLength);

    /**
     * The BCU's main processing loop. This is like the application's loop() function,
     * and is called automatically by main() when the BCU is activated with bcu.begin().
     */
    virtual void loop();

    /**
     * Allow an user provided memory mapper to store parameter data via memory write / read
     * @param mapper - a pointer to an instance of a MemMapper object
     */
    void setMemMapper(MemMapper *mapper);

    /**
     * Returns a pointer to the instance of the MemMapper object of the BCU
     * @return a pointer to the instance of the MemMapper object, in case of error return is nullptr
     */
    MemMapper* getMemMapper();

    /**
     * Set a callback class to notify the user program of some events
     */
    void setUsrCallback(UsrCallback *callback);

    /**
     * End using the EIB bus coupling unit.
     */
    virtual void end();

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
     */
    virtual void _begin();
    /**
     * Process a unicast telegram with our physical address as destination address.
     * The telegram is stored in sbRecvTelegram[].
     *
     * When this function is called, the sender address is != 0 (not a broadcast).
     *
     * @param apci - the application control field
     */
    void processDirectTelegram(int apci);

    /**
     * Process a device-descriptor-read request.
     *
     * @param id - the device-descriptor type ID
     *
     * @return True on success, false on failure
     */
    bool processDeviceDescriptorReadTelegram(int id);
    /**
     * Process a APCI_MEMORY_WRITE_PDU
     * see KNX Spec. 3/3/7 §3.5.4 p.73 A_Memory_Write-service
     *
     * @param addressStart - memory start address
     * @param payLoad - data to write into the memory
     * @param lengthPayLoad - length of data/payload
     *
     * @return true if successfully written, otherwise false
     */
    bool processApciMemoryWritePDU(int addressStart, byte *payLoad, int lengthPayLoad);

    /**
     * Process a APCI_MEMORY_READ_PDU
     * see KNX Spec. 3/3/7 §3.5.3 p.71 A_Memory_Read-service
     *
     * @param addressStart - memory start address
     * @param payLoad - buffer to write data read from memory
     * @param lengthPayLoad - length of data/payload to read
     *
     * @return true if successfully read, otherwise false
     */
    bool processApciMemoryReadPDU(int addressStart, byte *payLoad, int lengthPayLoad);

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
    bool processApciMemoryOperation(int addressStart, byte *payLoad, int lengthPayLoad, const bool readMem);

    /**
     * Process a APCI_MASTER_RESET_PDU
     * see KNX Spec. 3/5/2 §3.7.1.2 p.64 A_Restart
     *
     * @param apci          APCI to process
     * @param senderSeqNo   The TL layer 4 sequence number of the sender
     * @param eraseCode     eraseCode of the @ref APCI_MASTER_RESET_PDU telegram
     * @param channelNumber channelNumber of the @ref APCI_MASTER_RESET_PDU telegram
     * @note sendTelegram is accessed and changed inside the function to prepare a @ref APCI_MASTER_RESET_RESPONSE_PDU
     *
     * @return true if a restart shall happen, otherwise false
     */
    bool processApciMasterResetPDU(int apci, const int senderSeqNo, byte eraseCode, byte channelNumber);

    /**
     * @brief Performs a system reset by calling @ref NVIC_SystemReset
     * @details Before the reset a USR_CALLBACK_RESET is send to the application,
     *          the UserEprom and memMapper are written to flash.
     * @warning This function will never return.
     */
    void softSystemReset();

    // The method begin_BCU() is renamed during compilation to indicate the BCU type.
    // If you get a link error then the library's BCU_TYPE is different from your application's BCU_TYPE.
    void begin_BCU(int manufacturer, int deviceType, int version);

private:
    MemMapper *memMapper;
    UsrCallback *usrCallback;
    bool sendGrpTelEnabled;        //!< Sending of group telegrams is enabled. Usually set, but can be disabled.
    unsigned int groupTelWaitMillis;
    unsigned int groupTelSent;
};


#define  MAX_GROUP_TEL_PER_SECOND  28
#define  DEFAULT_GROUP_TEL_WAIT_MILLIS  1000/MAX_GROUP_TEL_PER_SECOND

//
//  Inline functions
//
inline void BCU::setMemMapper(MemMapper *mapper)
{
    memMapper = mapper;
}

inline MemMapper* BCU::getMemMapper()
{
    return memMapper;
}

inline void BCU::setUsrCallback(UsrCallback *callback)
{
    usrCallback = callback;
}

inline void BCU::enableGroupTelSend(bool enable)
{
    sendGrpTelEnabled = enable;
}
/**
 * The sending circuit of the Controller has limited capability for sending low bits.
 *
 * In order to avoid a thermal destruction (more than 1W power dissipation of the Transistor and 150mW of the 5R6 Resistor
 * a limit of max 28 telegrams per second should be set (assuming an average distribution of 50% low bits within telegrams
 * of max length).
 *
 * @param limit
 */
inline void BCU::setGroupTelRateLimit(unsigned int limit)
{
 if ((limit > 0) && (limit <= MAX_GROUP_TEL_PER_SECOND))
     groupTelWaitMillis = 1000/limit;
 else
     groupTelWaitMillis = DEFAULT_GROUP_TEL_WAIT_MILLIS ;
}

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /*sblib_bcu_h*/
