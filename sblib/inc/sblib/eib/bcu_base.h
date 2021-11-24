/*
 *  bcubase.h - BCU specific stuff.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_BcuBase_h
#define sblib_BcuBase_h

#include <sblib/types.h>
#include <sblib/eib/com_objects.h>
#include <sblib/utils.h>
#include <sblib/mem_mapper.h>
#include <sblib/usr_callback.h>
#include <sblib/eib/userRam.h>
#include <sblib/eib/userEeprom.h>
#include <sblib/eib/userRam.h>
#include <sblib/eib/addr_tables.h>
#include <sblib/timer.h>
#include <sblib/debounce.h>

class Bus;

/**
 * Class for controlling all BCU related things.
 *
 * In order to use the EIB bus, you need to call bcu.begin() once in your application's
 * setup() function.
 */
class BcuBase
{
public:
    BcuBase(UserRam* userRam, UserEeprom* userEeprom, ComObjects* comObjects, AddrTables* addrTables);
    virtual ~BcuBase() = default;

    void setRxPin(int rxPin);
    void setTxPin(int txPin);
    void setTimer(Timer& timer);
    void setCaptureChannel(TimerCapture captureChannel);
    void setProgPin(int prgPin);
    void setProgPinInverted(int prgPinInv);

    /**
     * End using the EIB bus coupling unit.
     */
    virtual void end();

    /**
     * Set our own physical address. Normally the physical address is set by ETS when
     * programming the device.
     *
     * @param addr - the physical address
     */
    virtual void setOwnAddress(int addr) = 0;

    /**
     * Get our own physical address.
     */
    int ownAddress() const;

    /**
     * Test if the programming mode is active. This is also indicated
     * by the programming mode LED.
     *
     * @return True if the programming mode is active, false if not.
     */
    bool programmingMode();

    /**
     * Test if the user application is active. The application is active if the
     * application layer is active in userRam.status, the programming mode is not
     * active, and the run error in userEeprom.runError is 0xff (no error).
     *
     * @return True if the user application is active, false if not.
     */
    virtual bool applicationRunning() const = 0;

    /**
     * Test if a direct data connection is open.
     *
     * @return True if a connection is open, false if not.
     */
    bool directConnection() const;

    /**
     * Process the received telegram from bus.telegram.
     * Called by main()
     */
    virtual void processTelegram();

    /**
     * Get the mask version.
     * Usually 0x0012 for BCU1, 0x0020 for BCU2.
     */
    virtual const unsigned short getMaskVersion() const = 0;

    virtual const char* getBcuType() const = 0;

    /**
     * The BCU's main processing loop. This is like the application's loop() function,
     * and is called automatically by main() when the BCU is activated with bcu.begin().
     */
    virtual void loop();

    /**
     * Get the read-only CommObjectTable address, which can be set calling Begin(...)
     * @return The read-only CommObjectTable address which can't be changed by KNX telegrams
     */
    word getCommObjectTableAddressStatic() const;

    /**
     * A buffer for sending telegrams. This buffer is considered library private
     * and should rather not be used by the application program.
     */
    byte *sendTelegram;

    /**    z = bcu.getOwnAddress();
     *
     * The pin where the programming LED + button are connected. The default pin
     * is PIO2_0. This variable may be changed in setup(), if required. If set
     * to 0, the programming LED + button are not handled by the library.
     */
    int progPin;

    /**
     * Programming LED output inverted: If set to 1 the programming LED output is
     * being inverted
     */
    int progPinInv;

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

    UserEeprom* userEeprom;
    UserRam* userRam;
    ComObjects* comObjects;
    AddrTables* addrTables;

    virtual int TelegramSize();

    byte* userMemoryPtr(int addr);

    /**
     * Returns a pointer to the instance of the MemMapper object of the BCU
     * @return a pointer to the instance of the MemMapper object, in case of error return is nullptr
     */
    MemMapper* getMemMapper();

    Bus* bus;

protected:
    // The method begin_BCU() is renamed during compilation to indicate the BCU type.
    // If you get a link error then the library's BCU_TYPE is different from your application's BCU_TYPE.
    virtual void begin_BCU(int manufacturer, int deviceType, int version) = 0;
    /*
     * Special initialization for the BCU
     */
    virtual void _begin();

    /**
     * Creates a len_hash wide hash of the uid.
     * Hash will be generated in provided hash buffer
     *
     * @param uid - LPC-serial (128bit GUID) returned by iapReadUID() which will be hashed
     * @param len_uid - size of uid  (normally 16 byte)
     * @param hash - buffer for generated hash
     * @param len_hash - size of provided hash buffer (normally 6byte/48bit for EIB)
     * @return True if hash successfully created, false if not.
     */
    int hashUID(byte* uid, const int len_uid, byte* hash, const int len_hash);

    /**
     * @brief Set or unset the programming mode of the bcu
     *
     * @param  new programming button state
     * @return true if successful, otherwise false
     */
    bool setProgrammingMode(bool newMode);

    Debouncer progButtonDebouncer; //!< The debouncer for the programming mode button.
    bool enabled;                  //!< The BCU is enabled. Set by bcu.begin().
    int  connectedAddr;            //!< Remote address of the connected partner.
    byte sendCtrlTelegram[8];      //!< A short buffer for connection control telegrams.
    int  connectedSeqNo;           //!< Sequence number for connected data telegrams.
    unsigned int connectedTime;    //!< System time of the last connected telegram.
    bool incConnectedSeqNo;        //!< True if the sequence number shall be incremented on ACK.
    int lastAckSeqNo;              //!< Last acknowledged sequence number

    /**
     * Process a unicast connection control telegram with our physical address as
     * destination address. The telegram is stored in sbRecvTelegram[].
     *
     * When this function is called, the sender address is != 0 (not a broadcast).
     *
     * @param tpci - the transport control field
     */
    void processConControlTelegram(int tpci);

    /**
     * Process a unicast telegram with our physical address as destination address.
     * The telegram is stored in sbRecvTelegram[].
     *
     * When this function is called, the sender address is != 0 (not a broadcast).
     *
     * @param apci - the application control field
     */
    virtual void processDirectTelegram(int apci) = 0;

    /**
     * Send a connection control telegram.
     *
     * @param cmd - the transport command, see SB_T_xx defines
     * @param senderSeqNo - the sequence number of the sender, 0 if not required
     */
    void sendConControlTelegram(int cmd, int senderSeqNo);

    /**
     * Process a device-descriptor-read request.
     *
     * @param id - the device-descriptor type ID
     *
     * @return True on success, false on failure
     */
    bool processDeviceDescriptorReadTelegram(int id);

    int connectedTo();

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

    void cpyFromUserRam(unsigned int address, unsigned char * buffer, unsigned int count);
    void cpyToUserRam(unsigned int address, unsigned char * buffer, unsigned int count);

    MemMapper *memMapper;
    UsrCallback *usrCallback;
    bool sendGrpTelEnabled;        //!< Sending of group telegrams is enabled. Usually set, but can be disabled.
    unsigned int groupTelWaitMillis;
    unsigned int groupTelSent;

    word commObjectTableAddressStatic;       //!> The read-only CommObjectTable address which can't be changed by KNX telegrams

    volatile int ownAddr;                 //!< Our own physical address on the bus
};

//
//  Inline functions
//

#define  MAX_GROUP_TEL_PER_SECOND  28
#define  DEFAULT_GROUP_TEL_WAIT_MILLIS  1000/MAX_GROUP_TEL_PER_SECOND

//
//  Inline functions
//

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /*sblib_bcu_h*/
