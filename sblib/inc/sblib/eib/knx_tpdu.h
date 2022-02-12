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

#ifndef SBLIB_EIB_KNX_TPDU_H_
#define SBLIB_EIB_KNX_TPDU_H_

#include <sblib/types.h>

inline byte sequenceNumber(unsigned char *telegram)
{
    byte tpci = telegram[6];
    if (tpci != 0xFF)
    {
        return ((tpci & T_SEQUENCE_NUMBER_Msk) >> T_SEQUENCE_NUMBER_FIRST_BIT_Pos);
    }
    else
    {
        return (-1);
    }
}




#endif /* SBLIB_EIB_KNX_TPDU_H_ */
/** @}*/
