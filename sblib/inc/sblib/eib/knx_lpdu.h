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

#ifndef SBLIB_EIB_LPDU_H_
#define SBLIB_EIB_LPDU_H_

#include <sblib/types.h>

#define LPDU_CONTROL_BYTE           (0)
#define LPDU_SENDER_HIGH_BYTE       (1)
#define LPDU_SENDER_LOW_BYTE        (2)
#define LPDU_DESTINATION_HIGH_BYTE  (3)
#define LPDU_DESTINATION_LOW_BYTE   (4)


inline byte controlByte(unsigned char *telegram)
{
    return (telegram[LPDU_CONTROL_BYTE]);
}

inline bool isRepeated(unsigned char *telegram)
{
    // 5.bit not set => repeated
    byte cByte = controlByte(telegram);
    return ((cByte & (1 << 5)) == 0);
}

inline byte setRepeated(byte controlByte, bool repeated)
{
    byte mask = (1 << 5);
    if (repeated)
    {
        // 5.bit not set => repeated
        mask = ~mask;
        return (controlByte & mask);
    }
    else
    {
        return (controlByte | mask);
    }
}

inline unsigned short senderAddress(unsigned char *telegram)
{
    return (unsigned short)((telegram[LPDU_SENDER_HIGH_BYTE] << 8) | telegram[LPDU_SENDER_LOW_BYTE]);
}

inline unsigned short destinationAddress(unsigned char *telegram)
{
    return (unsigned short)((telegram[LPDU_DESTINATION_HIGH_BYTE] << 8) | telegram[LPDU_DESTINATION_LOW_BYTE]);
}



#endif /* SBLIB_EIB_LPDU_H_ */
/** @}*/
