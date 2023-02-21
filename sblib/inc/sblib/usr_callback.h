/*
 * usr_callback.h
 *
 *  Created on: May 26, 2016
 *      Author: Florian Voelzke
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 *
 */

#ifndef SBLIB_USR_CALLBACK_H_
#define SBLIB_USR_CALLBACK_H_

#include <stdint.h>

enum class UsrCallbackType : uint8_t {
    reset = 1, //!< Wird aufgerufen direkt vor einem µC-Reset
    flash = 2, //!< Wird aufgerufen allgemein wenn Änderungen am User_Eeprom vorgenommen worden sind
    bcu_end = 3, //!< Kommt eigentlich nicht vor, wird bislang nicht mal bei einem Busspannungsausfall aufgerufen. Es können aber die Schritte wie bei den anderen Fällen ausgeführt werden.
    recallAppStartup = 4,
    recallAppInit = 5,
    recallAppOther = 6,
    storeAppDownload = 0x80,
    storeAppBusVoltageFail = 0x81
};

class UsrCallback
{
public:

    virtual void Notify(UsrCallbackType type)=0;
};

#endif
