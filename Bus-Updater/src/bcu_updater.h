/*
 * bcu_updater.h
 *
 *  Created on: 15.07.2015
 *      Author: glueck
 */

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

class BcuUpdate: public BcuBase
{
public:
    virtual void processTelegram();
    bool progPinStatus();
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
};

inline bool BcuUpdate::progPinStatus()
{
    return programmingMode();
}

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /* BCU_UPDATER_H_ */
