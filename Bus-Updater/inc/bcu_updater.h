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
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2022
 * @bug No known bugs.
 ******************************************************************************/

/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 3 as
 published by the Free Software Foundation.
 -----------------------------------------------------------------------------*/

#ifndef BCU_UPDATER_H_
#define BCU_UPDATER_H_

#include <sblib/internal/variables.h>
#include <sblib/timeout.h>
#include <sblib/io_pin_names.h>
#include <sblib/eib/bcu_base.h>
#include "update.h"

// Rename the method begin_BCU() of the class BCU to indicate the BCU type. If you get a
// link error then the library's BCU_TYPE is different from the application's BCU_TYPE.
#define begin_BCU  CPP_CONCAT_EXPAND(begin_,BCU_NAME)

class BcuUpdate: public BcuBase
{
public:
    using BcuBase::setProgrammingMode; // make it public so we can use it in bootloader.cpp

protected:
    unsigned char processApci(ApciCommand apciCmd, const int senderAddr, const int senderSeqNo, bool *sendResponse, unsigned char *telegram, unsigned short telLength) override;
    bool processGroupAddressTelegram(ApciCommand apciCmd, unsigned short groupAddress, unsigned char *telegram, unsigned short telLength) override;
    bool processBroadCastTelegram(ApciCommand apciCmd, unsigned char *telegram, unsigned short telLength) override;
};

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /* BCU_UPDATER_H_ */

/** @}*/
