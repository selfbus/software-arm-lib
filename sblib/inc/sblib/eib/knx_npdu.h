/**************************************************************************//**
 * @addtogroup SBLIB_MAIN_GROUP Selfbus KNX-Library
 * @defgroup SBLIB_SUB_GROUP_KNX KNX NPDU Handling
 * @ingroup SBLIB_MAIN_GROUP
 * @brief   
 * @details 
 *
 *
 * @{
 *
 * @file   knx_npdu.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 ---------------------------------------------------------------------------*/

#ifndef SBLIB_KNX_NPDU_H_
#define SBLIB_KNX_NPDU_H_

#define NPDU_CONTROL_BYTE           (5)

enum NPDU
{
    N_DATA_INDIVIDUAL,
    N_DATA_GROUP,
    N_DATA_BROADCAST,
    N_DATA_SYSTEM_BROADCAST
};

enum Hop_count_type
{

};

/**
 * Get the size of a telegram, including the protocol header but excluding
 * the checksum byte. The size is calculated by getting the length from byte 5 of the
 * telegram and adding 7 for the protocol overhead.
 *
 * @param tel - the telegram to get the size
 *
 * @return The size of the telegram, excluding the checksum byte.
 */
#define telegramSize(tel) (7 + (tel[5] & 15)) //FIXME telegramSize accesses tel[5] without any check


#endif /* SBLIB_KNX_NPDU_H_ */
/** @}*/
