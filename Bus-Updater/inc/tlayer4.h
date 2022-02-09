/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Main Selfbus KNX-Library
 * @defgroup SBLIB_SUB_GROUP_1 Sub KNX Transport layer 4
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   tlayer4.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef TLAYER4_H_
#define TLAYER4_H_

#include <sblib/eib/bcu_base.h>

#define TL4_CONNECTION_TIMEOUT_MS (6000) //!< Transport layer 4 connection timeout in milliseconds
#define TL4_ACK_TIMEOUT_MS        (3000) //!< Transport layer 4 T_ACK/T_NACK timeout in milliseconds ///\todo not used in Style 1

#ifdef DEBUG
#   define LONG_PAUSE_THRESHOLD_MS (500)
#endif

extern unsigned short telegramCount;   //!< number of telegrams since system reset
extern unsigned short disconnectCount; //!< number of disconnects since system reset

///\todo remove after bugfix and on release
extern unsigned short repeatedTelegramCount;
extern unsigned short repeatedIgnoredTelegramCount;
///\todo end of remove after bugfix and on release


/**
 * @brief Implementation of the KNX transportation layer 4 Style 1 Rationalised
 *
 */
class TLayer4: public BcuBase
{
public:
    /** The states of the transport layer 4 state machine Style 1 rationalised */
    enum TL4State
    {
        CLOSED,
        OPEN_IDLE,
        OPEN_WAIT
    };

    TLayer4();
    virtual ~TLayer4() = default;

    virtual void _begin();

    /**
     * @brief pre-processes the received telegram from bus.telegram.
     *        Only telegrams matching @ref destAddr == @ref bus.ownAddress()
     *        are processed.
     */
    virtual void processTelegram();

    /**
     * The BCU's main processing loop. This is like the application's loop() function,
     * and is called automatically by main() when the BCU is activated with bcu.begin().
     */
    virtual void loop();

    /**
     * Test if a direct data connection is open.
     *
     * @return True if a connection is open, false if not.
     */
    bool directConnection() const;

protected:
    /**
     * @brief Reset the TP Layer 4 connection to CLOSED
     *
     */
    virtual void resetConnection();

    /**
     * Send a connection control telegram.
     *
     * @param cmd - the transport command, see SB_T_xx defines
     * @param address - the knx address to send the telegram to
     * @param senderSeqNo - the sequence number of the sender, 0 if not required
     */
    virtual void sendConControlTelegram(int cmd, unsigned int address, int senderSeqNo);

    virtual unsigned char processApci(int apci, const int senderAddr, const int senderSeqNo, bool * sendTel, unsigned char * data);

private:
    bool setTL4State(TLayer4::TL4State newState);

    /**
     * Process a unicast connection control telegram with our physical address as
     * destination address. The telegram is stored in sbRecvTelegram[].
     *
     * When this function is called, the sender address is != 0 (not a broadcast).
     *
     * @param tpci - the transport control field
     */
    void processConControlTelegram(const unsigned int senderAddr, const unsigned int tpci);

    /**
     * @brief Process a TP Layer 4 @ref T_CONNECT_PDU
     * @details T_Connect.req see KNX Spec. 2.1 3/3/4 page 13
     *
     * @param senderAddr physical KNX address of the sender
     * @return true if successful, otherwise false
     */
    bool processConControlConnectPDU(int senderAddr);

    /**
     * @brief Process a TP Layer 4 @ref T_DISCONNECT_PDU
     * @details T_Disconnect.req see KNX Spec. 2.1 3/3/4 page 14
     *
     * @param senderAddr physical KNX address of the sender
     * @return true if successful, otherwise false
     */
    bool processConControlDisconnectPDU(int senderAddr);

    /**
     * @brief Process a TP Layer 4 @ref T_ACK_PDU and @ref T_NACK_PDU
     * @details T_ACK-PDU & T_NAK-PDU see KNX Spec. 2.1 3/3/4 page 6
     *
     * @param senderAddr physical KNX address of the sender
     * @param tpci the TPCI to process
     * @return true if successful, otherwise false
     */
    bool processConControlAcknowledgmentPDU(int senderAddr, int tpci);

    /**
     * Process a unicast telegram with our physical address as destination address.
     * The telegram is stored in sbRecvTelegram[].
     *
     * When this function is called, the sender address is != 0 (not a broadcast).
     *
     * @param apci - the application control field
     */
    void processDirectTelegram(int apci);

    void actionA00Nothing();
    void actionA01Connect(unsigned int address);
    bool actionA02sendAckPduAndProcessApci(int apci, const int seqNo, unsigned char * data);
    void actionA03sendAckPduAgain(const int seqNo);

    /**
     * @brief Performs action A5 as described in the KNX Spec.
     */
    void actionA05DisconnectUser();

    /**
     * @brief Performs action A6 as described in the KNX Spec.
     *        Send a @ref T_DISCONNECT_PDU to @ref connectedAddr with system priority and Sequence# = 0
     */
    void actionA06Disconnect();

    /**
     * @brief Sends the direct telegram which is provided in global buffer @ref sendTelegram
     * @param senderSeqNo Senders sequence number
     *
     * @return true if sequence number was added, otherwise false
     */
    void actionA07SendDirectTelegram();
    void actionA08IncrementSequenceNumber();

    /**
     * @brief Performs action A10 as described in the KNX Spec.
     *        Send a @ref T_DISCONNECT_PDU to @ref address with system priority and Sequence# = 0
     *
     * @param address KNX address to send the @ref T_DISCONNECT_PDU
     */
    void actionA10Disconnect(unsigned int address);

    TLayer4::TL4State state = TLayer4::CLOSED;
    byte seqNoSend = -1;
    byte seqNoRcv = -1;
    bool telegramReadyToSend = false;

    bool checkValidRepeatedTelegram();  ///\todo remove after fix in Bus and on release
    void copyTelegram();
    byte lastTelegram[Bus::TELEGRAM_SIZE];
    int lastTelegramLength = 0;
};

inline bool TLayer4::directConnection() const
{
    return state != TLayer4::CLOSED;
}

#endif /* TLAYER4_H_ */
/** @}*/
