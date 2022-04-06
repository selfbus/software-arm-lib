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

#define NPDU_CONTROL_BYTE           (0)

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



#endif /* SBLIB_KNX_NPDU_H_ */
/** @}*/
