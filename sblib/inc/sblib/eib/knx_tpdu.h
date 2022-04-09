/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Selfbus KNX-Library
 * @defgroup SBLIB_SUB_GROUP_KNX KNX TPDU Handling
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   knx_tpdu.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef SBLIB_KNX_TPDU_H_
#define SBLIB_KNX_TPDU_H_

#include <sblib/types.h>

#define TPDU_HIGH_BYTE (5)
#define TPDU_LOW_BYTE (6)

/**
 * Transport commands (see KNX 2.1 3/3/4 p.6 Transport Control Field)
 */
enum TPDU
{
    T_CONNECT_PDU       = 0x80,         //!< T_Connect-PDU
    T_DISCONNECT_PDU    = 0x81,         //!< T_Disconnect-PDU
    T_ACK_PDU           = 0xc2,         //!< T_ACK-PDU
    T_NACK_PDU          = 0xc3          //!< T_NAK-PDU
};

/**
 *  Transport command bit positions and masks
 */
enum
{
    T_CONNECTION_CTRL_COMMAND_Pos   = 7,                                        //!< Position of the control command bit
    T_CONNECTION_CTRL_COMMAND_Msk   = (1UL << T_CONNECTION_CTRL_COMMAND_Pos),   //!< Bitmask to check for an connection control command

    T_ACKNOWLEDGE_Msk               = 0xC2,                                     //!< Bitmask to check for an @ref T_ACK_PDU or @ref T_NACK_PDU
    T_NACK_Pos                      = 0,                                        //!< Position of the @ref T_NACK_PDU bit
    T_NACK_Msk                      = (1UL << T_NACK_Pos),                      //!< Bitmask to check for an  @ref T_NACK_PDU bit

    T_IS_SEQUENCED_Pos              = 6,                                        //!< Position of the TPDU has sequence number bit
    T_IS_SEQUENCED_Msk              = (1UL << T_IS_SEQUENCED_Pos),              //!< Bitmask to check TPDU has a sequence number

    T_SEQUENCE_NUMBER_Msk           = 0x3c,                                     //!< Bitmask to get the sequence number of an TPDU
    T_SEQUENCED_COMMAND             = 0x40,                                     //!< T_SEQUENCED_COMMAND
    T_SEQUENCE_NUMBER_FIRST_BIT_Pos = 2,                                        //!< Position of first bit of the sequence number
    T_SEQUENCE_NUMBER_FIRST_BIT_Msk = (1UL << T_SEQUENCE_NUMBER_FIRST_BIT_Pos), //!< Bitmask to get the first bit of the sequence number

    T_GROUP_PDU                     = 0x00,                                     //!< T_GROUP_PDU
    T_GROUP_ADDRESS_FLAG_Pos        = 7,                                        //!< Position of address type flag bit
    T_GROUP_ADDRESS_FLAG_Msk        = (1UL << T_GROUP_ADDRESS_FLAG_Pos)         //!< Bitmask to check for an group address
};

inline byte sequenceNumber(unsigned char *telegram)
{
    byte tpci = telegram[TPDU_LOW_BYTE];
    if (tpci != 0xFF)
    {
        return ((tpci & T_SEQUENCE_NUMBER_Msk) >> T_SEQUENCE_NUMBER_FIRST_BIT_Pos);
    }
    else
    {
        return (-1);
    }
}

inline void setSequenceNumber(unsigned char *telegram, byte newSequenceNumber)
{
    telegram[TPDU_LOW_BYTE] &= static_cast<byte>(~T_SEQUENCE_NUMBER_Msk);
    telegram[TPDU_LOW_BYTE] |= static_cast<byte>(newSequenceNumber << T_SEQUENCE_NUMBER_FIRST_BIT_Pos);
    telegram[TPDU_LOW_BYTE] |= T_SEQUENCED_COMMAND; // set sequenced flag
}



#endif /* SBLIB_KNX_TPDU_H_ */
/** @}*/
