/*
 *  app_main.h - The application's main header file.
 *
 *  Copyright (c) 2024 Oliver Stefan <o.stefan252@googlemail.com>
 *     last change H. Rauch for example analog in the sblib
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef INC_APP_MAIN_H_
#define INC_APP_MAIN_H_

#ifdef BUSFAIL
#    include <sblib/usr_callback.h>
#    include <sblib/eib/bus_voltage.h>
#    include "app_nov_settings.h"
#    include "channel.h"
#endif


/* define class for BCU callback in the App
 *
 */
#ifdef BUSFAIL
class AppADCReadCallback: public BCUADCReadCallback {
public:
    virtual int ADCReadCount(unsigned int adcChannel );
};

class AppUserSaveCallback: public UsrCallback {
public:
    virtual void Notify(UsrCallbackType type);
};
#endif


#endif /* INC_APP_MAIN_H_ */
