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

/**
 * Class for controlling all BCU related things.
 *
 * In order to use the EIB bus, you need to call bcu.begin() once in your application's
 * setup() function.
 */
class BCU : public BCU_Base
{
public:
    virtual void processTelegram();

    /**
     * The BCU's main processing loop. This is like the application's loop() function,
     * and is called automatically by main() when the BCU is activated with bcu.begin().
     */
    virtual void loop();
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
};


#endif /*sblib_bcu_h*/
