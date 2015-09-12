/*
 *  bcu.h - BCU specific stuff.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_bcu_h
#define sblib_bcu_h

#include <sblib/eib/bcu_base.h>
#include <sblib/types.h>
#include <sblib/eib/bus.h>
#include <sblib/eib/bcu_type.h>
#include <sblib/eib/properties.h>
#include <sblib/eib/user_memory.h>
#include <sblib/utils.h>
#include <sblib/mem_mapper.h>


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
    virtual void processTelegram();

    /**
     * The BCU's main processing loop. This is like the application's loop() function,
     * and is called automatically by main() when the BCU is activated with bcu.begin().
     */
    virtual void loop();

    int connectedTo();

    /**
     * Allow an user provided memory mapper to store parameter data via memory write / read
     * @param mapper - a pointer to an instance of a MemMapper object
     */
    void setMemMapper(MemMapper *mapper);

    /**
     * A buffer for sending telegrams. This buffer is considered library private
     * and should rather not be used by the application program.
     */
    byte sendTelegram[Bus::TELEGRAM_SIZE];

    /**
     * The pin where the programming LED + button are connected. The default pin
     * is PIO1_5. This variable may be changed in setup(), if required. If set
     * to 0, the programming LED + button are not handled by the library.
     */
    int progPin;

    /**
     * End using the EIB bus coupling unit.
     */
    virtual void end();

protected:
    /*
     * Special initialization for the BCU
     */
    virtual void _begin();
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
    void processDirectTelegram(int apci);

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

    // The method begin_BCU() is renamed during compilation to indicate the BCU type.
    // If you get a link error then the library's BCU_TYPE is different from your application's BCU_TYPE.
    void begin_BCU(int manufacturer, int deviceType, int version);

private:
    MemMapper *memMapper;
};


//
//  Inline functions
//

inline int BCU::connectedTo()
{
    return connectedAddr;
}

inline void BCU::setMemMapper(MemMapper *mapper)
{
    memMapper = mapper;
}

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /*sblib_bcu_h*/
