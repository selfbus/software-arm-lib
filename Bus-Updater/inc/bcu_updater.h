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

#include "bcu_layer4.h"
#include <sblib/internal/variables.h>
#include <sblib/timeout.h>
#include <sblib/io_pin_names.h>
#include "update.h"

// Rename the method begin_BCU() of the class BCU to indicate the BCU type. If you get a
// link error then the library's BCU_TYPE is different from the application's BCU_TYPE.
#define begin_BCU  CPP_CONCAT_EXPAND(begin_,BCU_NAME)

class BcuUpdate: public BcuLayer4
{
public:
    using BcuLayer4::setProgrammingMode; // make it public so we can use it in bootloader.cpp

protected:
    /**
     * @brief Reset the TP Layer 4 connection to CLOSED
     *
     */
    virtual void resetConnection();

    virtual unsigned char processApci(int apci, const int senderAddr, const int senderSeqNo, bool * sendTel, unsigned char * data);


     /**
      * Process a APCI_MASTER_RESET_PDU
      * see KNX Spec. 3/5/2 §3.7.1.2 p.64 A_Restart
      *
      * @param apci          APCI to process
      * @param senderSeqNo   The TL layer 4 sequence number of the sender
      * @param eraseCode     eraseCode of the @ref APCI_MASTER_RESET_PDU telegram
      * @param channelNumber channelNumber of the @ref APCI_MASTER_RESET_PDU telegram
      * @note sendTelegram is accessed and changed inside the function to prepare a @ref APCI_MASTER_RESET_RESPONSE_PDU
      *
      * @return true if a restart shall happen, otherwise false
      */
     bool processApciMasterResetPDU(int apci, const int senderSeqNo, byte eraseCode, byte channelNumber);

     /**
      * Process a device-descriptor-read request.
      *
      * @param id - the device-descriptor type ID
      *
      * @return True on success, false on failure
      */
     bool processDeviceDescriptorReadTelegram(int id);
};

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /* BCU_UPDATER_H_ */

/** @}*/
