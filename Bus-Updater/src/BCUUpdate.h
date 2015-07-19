/*
 * BCUUpdate.h
 *
 *  Created on: 15.07.2015
 *      Author: glueck
 */

#ifndef BCUUPDATE_H_
#define BCUUPDATE_H_

#include <sblib/eib/BCU_Base.h>
#include <sblib/eib/apci.h>
#include <sblib/internal/variables.h>

class BCU_Update : public BCU_Base
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

inline bool BCU_Update::progPinStatus()
{
	return progButtonDebouncer.value();
}
#endif /* BCUUPDATE_H_ */
