/*
 *  bcu_base.h - Minimum stuff for a BCU
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_BcuBase_h
#define sblib_BcuBase_h

#include <sblib/types.h>
#include <sblib/utils.h>
#include <sblib/eib/userRam.h>
#include <sblib/eib/addr_tables.h>
#include <sblib/eib/com_objects.h>
#include <sblib/timer.h>
#include <sblib/debounce.h>
#include <sblib/eib/knx_tlayer4.h>

class Bus;

/**
 * Class for controlling minimum BCU related things.
 */
class BcuBase: public TLayer4
{
public:
    Bus* bus;
    BcuBase(UserRam* userRam, AddrTables* addrTables);
    BcuBase() = delete;
    ~BcuBase() = default;

    /**
     * End using the EIB bus coupling unit.
     */
    void end();

    /**
     * Test if the programming mode is active. This is also indicated
     * by the programming mode LED.
     *
     * @return True if the programming mode is active, false if not.
     */
    bool programmingMode() const;

    /**
     * Test if the user application is active. The application is active if the
     * application layer is active in userRam.status, the programming mode is not
     * active, and the run error in userEeprom.runError is 0xff (no error).
     *
     * @return True if the user application is active, false if not.
     */
    virtual bool applicationRunning() const = 0;

    /**
     * The BCU's main processing loop. This is like the application's loop() function,
     * and is called automatically by main() when the BCU is activated with bcu.begin().
     */
    void loop() override;

    /**
     *
     * The pin where the programming LED + button are connected. The default pin
     * is PIO2_0. This variable may be changed in setup(), if required. If set
     * to 0, the programming LED + button are not handled by the library.
     */
    int progPin;

    /**
     * Programming LED output inverted: If set to 1 the programming LED output is
     * being inverted
     */
    int progPinInv;

     /**
      * @brief Performs a system reset by calling @ref NVIC_SystemReset
      * @details Before the reset a USR_CALLBACK_RESET is send to the application,
      *          the UserEprom and memMapper are written to flash.
      * @warning This function will never return.
      */
     virtual void softSystemReset();

    UserRam* userRam;
    AddrTables* addrTables;
    ComObjects* comObjects;

    virtual int maxTelegramSize();

protected:
    /**
     * Special initialization for the BCU
     */
    virtual void _begin() override;

    /**
     * @brief Set or unset the programming mode of the bcu
     *
     * @param  new programming button state
     * @return true if successful, otherwise false
     */
    bool setProgrammingMode(bool newMode);

    void sendApciIndividualAddressReadResponse();

    Debouncer progButtonDebouncer; //!< The debouncer for the programming mode button.

    void discardReceivedTelegram();
    void send(unsigned char* telegram, unsigned short length);

};

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /*sblib_BcuBase_h*/
