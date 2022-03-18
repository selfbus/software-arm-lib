/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Selfbus KNX-Library
 * @defgroup SBLIB_SUB_GROUP_KNX KNX LPDU Handling
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   knx_lpdu.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef SBLIB_KNX_LPDU_H_
#define SBLIB_KNX_LPDU_H_

#include <sblib/types.h>

#define LPDU_CONTROL_BYTE           (0)
#define LPDU_SENDER_HIGH_BYTE       (1)
#define LPDU_SENDER_LOW_BYTE        (2)
#define LPDU_DESTINATION_HIGH_BYTE  (3)
#define LPDU_DESTINATION_LOW_BYTE   (4)

#define MASK_REPEATED               (1 << 5)
#define MASK_FRAMETYPE              (1 << 7)

/**
 *  KNX Priority
 */
enum KNXPriority : byte
{
    PRIORITY_SYSTEM = 0,/**< PRIORITY_SYSTEM */
    PRIORITY_HIGH = 1,  /**< PRIORITY_HIGH */
    PRIORITY_ALARM = 2, /**< PRIORITY_ALARM */
    PRIORITY_LOW = 3    /**< PRIORITY_LOW */
};

/**
 *  KNX Frame format
 */
enum KNXFrameType
{
    FRAME_STANDARD,/**< FRAME_STANDARD */
    FRAME_EXTENDED /**< FRAME_EXTENDED */
};

void initLpdu(unsigned char *telegram, KNXPriority newPriority, bool newRepeated, KNXFrameType newFrameType);
byte controlByte(unsigned char *telegram);
bool isRepeated(unsigned char *telegram);
void setRepeated(unsigned char *telegram, bool repeated);
KNXPriority priority(unsigned char *telegram);
void setPriority(unsigned char *telegram, KNXPriority newPriority);
unsigned short senderAddress(unsigned char *telegram);
unsigned short destinationAddress(unsigned char *telegram);
void setDestinationAddress(unsigned char *telegram, unsigned short newDestinationAddress);
KNXFrameType frameType(unsigned char *telegram);
void setFrameType(unsigned char *telegram, KNXFrameType newFrameType);


inline void initLpdu(unsigned char *telegram, KNXPriority newPriority, bool newRepeated, KNXFrameType newFrameType)
{
    telegram[LPDU_CONTROL_BYTE] = 0xB0;
    setRepeated(telegram, newRepeated);
    setPriority(telegram, newPriority);
    setFrameType(telegram, newFrameType);
}

inline byte controlByte(unsigned char *telegram)
{
    return (telegram[LPDU_CONTROL_BYTE]);
}

inline bool isRepeated(unsigned char *telegram)
{
    // 5.bit not set => repeated
    return ((controlByte(telegram) & MASK_REPEATED) == 0);
}

inline void setRepeated(unsigned char *telegram, bool repeated)
{
    if (repeated)
    {
        // 5.bit not set => repeated
        telegram[LPDU_CONTROL_BYTE] &= ~MASK_REPEATED;
    }
    else
    {
        telegram[LPDU_CONTROL_BYTE] |= MASK_REPEATED;
    }
}

inline KNXPriority priority(unsigned char *telegram)
{
    // 3. and 4. bit control priority
    return ((KNXPriority)((controlByte(telegram) >> 2) & 0x03));
}

inline void setPriority(unsigned char *telegram, KNXPriority newPriority)
{
    telegram[LPDU_CONTROL_BYTE] =  (newPriority << 2) | (telegram[LPDU_CONTROL_BYTE] & 0xF3);
}

inline unsigned short senderAddress(unsigned char *telegram)
{
    return (unsigned short)((telegram[LPDU_SENDER_HIGH_BYTE] << 8) | telegram[LPDU_SENDER_LOW_BYTE]);
}

inline void setSenderAddress(unsigned char *telegram, unsigned short newSenderAddress)
{
    telegram[LPDU_SENDER_HIGH_BYTE] = (byte)(newSenderAddress >> 8);
    telegram[LPDU_SENDER_LOW_BYTE] = (byte)newSenderAddress;
}


inline unsigned short destinationAddress(unsigned char *telegram)
{
    return (unsigned short)((telegram[LPDU_DESTINATION_HIGH_BYTE] << 8) | telegram[LPDU_DESTINATION_LOW_BYTE]);
}

inline void setDestinationAddress(unsigned char *telegram, unsigned short newDestinationAddress)
{
    telegram[LPDU_DESTINATION_HIGH_BYTE] = (byte)(newDestinationAddress >> 8);
    telegram[LPDU_DESTINATION_LOW_BYTE] = (byte)newDestinationAddress;
}

inline KNXFrameType frameType(unsigned char *telegram)
{
    if (controlByte(telegram) & MASK_FRAMETYPE)
    {
        return FRAME_STANDARD;
    }
    else
    {
        return FRAME_EXTENDED;
    }
}

inline void setFrameType(unsigned char *telegram, KNXFrameType newFrameType)
{
    if (newFrameType == FRAME_EXTENDED)
    {
        // 7.bit not set => extended
        telegram[LPDU_CONTROL_BYTE] &= ~MASK_FRAMETYPE;
    }
    else
    {
        telegram[LPDU_CONTROL_BYTE] |= MASK_FRAMETYPE;
    }
}



#endif /* SBLIB_KNX_LPDU_H_ */
/** @}*/
