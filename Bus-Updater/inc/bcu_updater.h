/**************************************************************************//**
 * @addtogroup SBLIB_BOOTLOADER Selfbus Bootloader
 * @defgroup SBLIB_BOOTLOADER_BCU Bus coupling unit (BCU)
 * @ingroup SBLIB_BOOTLOADER
 * @brief    Bus coupling unit (BCU)
 * @details
 *
 * @{
 *
 * @file   bcu_updater.h
 * @author Martin Glueck <martin@mangari.org> Copyright (c) 2015
 * @author Stefan Haller Copyright (c) 2021
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#ifndef BCU_UPDATER_H_
#define BCU_UPDATER_H_

#include <sblib/eib/bcu_base.h>
#include <sblib/eib/apci.h>
#include <sblib/internal/variables.h>
#include <sblib/timeout.h>
#include <sblib/io_pin_names.h>
#include "update.h"

// Rename the method begin_BCU() of the class BCU to indicate the BCU type. If you get a
// link error then the library's BCU_TYPE is different from the application's BCU_TYPE.
#define begin_BCU  CPP_CONCAT_EXPAND(begin_,BCU_NAME)

#define BCU_DIRECT_CONNECTION_TIMEOUT_MS (6000) //!< BCU direct connection timeout in milliseconds

class BcuUpdate: public BcuBase
{
public:
    virtual void processTelegram();

    /**
     * The BCU's main processing loop. This is like the application's loop() function,
     * and is called automatically by main() when the BCU is activated with bcu.begin().
     */
    virtual void loop();

    bool progPinStatus();
    using BcuBase::setProgrammingMode; // makes it public so we can use it in bootloader.cpp
protected:
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
     * Process a unicast connection control telegram with our physical address as
     * destination address. The telegram is stored in sbRecvTelegram[].
     *
     * When this function is called, the sender address is != 0 (not a broadcast).
     *
     * @param tpci - the transport control field
     */
    void processConControlTelegram(int tpci);

    /**
     * Send a connection control telegram.
     *
     * @param cmd - the transport command, see SB_T_xx defines
     * @param senderSeqNo - the sequence number of the sender, 0 if not required
     */
    void sendConControlTelegram(int cmd, int senderSeqNo);

    /**
     * @brief Sends a A_Restart_Response-PDU
     * @details response to A_Restart-PDU KNX Spec. 3/5/2
     *
     * @param cmd         Transport command, usually APCI_RESTART_RESPONSEU
     * @param senderSeqNo The sequence number of the sender, 0 if not required
     * @param errorCode   if no error 0 otherwise error code
     * @param processTime Time in seconds the restart process will take
     */
     void sendRestartResponseControlTelegram(int senderSeqNo, int cmd, byte errorCode, unsigned int processTime);


};

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /* BCU_UPDATER_H_ */

/** @}*/
