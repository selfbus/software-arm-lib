/*
 *  bcubase.h - BCU specific stuff.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_BcuBase_h
#define sblib_BcuBase_h

#include <sblib/types.h>
#include <sblib/eib/knx_tlayer4.h>
#include <sblib/eib/bcu_type.h>
#include <sblib/eib/properties.h>
#include <sblib/eib/user_memory.h>
#include <sblib/utils.h>


// Rename the method begin_BCU() of the class BCU to indicate the BCU type. If you get a
// link error then the library's BCU_TYPE is different from the application's BCU_TYPE.
#define begin_BCU  CPP_CONCAT_EXPAND(begin_,BCU_NAME)

class BcuBase;

/**
 * The EIB bus coupling unit.
 */
extern BcuBase& bcu;


/**
 * Class for controlling all BCU related things.
 *
 * In order to use the EIB bus, you need to call bcu.begin() once in your application's
 * setup() function.
 */
class BcuBase: public TLayer4
{
public:
    BcuBase();
    virtual ~BcuBase() = default;

#if BCU_TYPE == BCU1_TYPE
    /**
     * Begin using the EIB bus coupling unit, and set the manufacturer-ID, device type,
     * program version and a optional read-only CommObjectTable address which
     * can't be changed by ETS/KNX telegrams
     *
     * @param manufacturer - the manufacturer ID (16 bit)
     * @param deviceType - the device type (16 bit)
     * @param version - the version of the application program (8 bit)
     */
    void begin(int manufacturer, int deviceType, int version);
#else
    /**
     * Begin using the EIB bus coupling unit, and set the manufacturer-ID, device type,
     * program version and a optional read-only CommObjectTable address which
     * can't be changed by ETS/KNX telegrams
     *
     * @param manufacturer - the manufacturer ID (16 bit)
     * @param deviceType - the device type (16 bit)
     * @param version - the version of the application program (8 bit)
     * @param readOnlyCommObjectTableAddress - optional (16bit), to set a read-only CommObjectTable address which won't be changed by KNX telegrams
     *                                         This is a workaround for ETS product databases, which send the "wrong" ComObjectTable address.
     *                                         Compare inside the extracted *.knxprod product database M-xxxx_x_xxxx-xx-xxxx.xml
     *                                         node <KNX/ManufacturerData/Manufacturer/ApplicationPrograms/ApplicationProgram/Static/ComObjectTable CodeSegment="M-xxxx_x-xxxx-xx-xxxx_xx-HHHH"
     *                                         HHHH = communication object table address in hexadecimal
     *                                         and
     *                                         node <KNX/ManufacturerData/Manufacturer/ApplicationPrograms/ApplicationProgram/Static/LoadProcedures/LdCtrlTaskSegment LsmIdx="3" Address="dddddd" />
     *                                         dddddd = communication object table address in decimal ETS wants us to use
     *                                         convert dddddd to hexadecimal => WWWW
     *                                         in case HHHH != WWWW ,
     *                                         use bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION, 0xHHHH) to set the correct read-only ComObjectTable address (HHHH)
     */
    void begin(int manufacturer, int deviceType, int version, word readOnlyCommObjectTableAddress = 0);
#endif

    /**
     * Set RxPin of board, must be called before begin method
     * @param rxPin pin definition
     */
    void setRxPin(int rxPin) {
        bus.rxPin=rxPin;
    }
    /**
     * Set TxPin of board, must be called before begin method
     * @param txPin pin definition
     */
    void setTxPin(int txPin) {
        bus.txPin=txPin;
    }
    /**
     * Set timer class, must be called before begin method
     * @param timer
     */
    void setTimer(Timer& timer) {
        bus.timer=timer;
    }
    /**
     * Set capture channel of processor, must be called before begin method
     * @param capture channel definition of processor
     */
    void setCaptureChannel(TimerCapture captureChannel) {
        bus.captureChannel=captureChannel;
    }
    /**
     * Set ProgPin of board, must be called before begin method
     * @param progPin Pin definition
     */
    void setProgPin(int prgPin) {
        progPin=prgPin;
        setFatalErrorPin(progPin);
    }
    /**
     * Set ProgPin output inverted, must be called before begin method
     * @param progPin output inverted
     */
    void setProgPinInverted(int prgPinInv) {
        progPinInv=prgPinInv;
    }
    /**
     * End using the EIB bus coupling unit.
     */
    virtual void end();

    /**
     * Set our own physical address. Normally the physical address is set by ETS when
     * programming the device.
     *
     * @param addr - the physical address
     */
    void setOwnAddress(int addr);

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
    bool applicationRunning() const;

    /**
     * Get the mask version.
     * Usually 0x0012 for BCU1, 0x0020 for BCU2.
     */
    unsigned short maskVersion();

    /**
     * The BCU's main processing loop. This is like the application's loop() function,
     * and is called automatically by main() when the BCU is activated with bcu.begin().
     */
    virtual void loop();

    /**
     * Get the read-only CommObjectTable address, which can be set calling Begin(...)
     * @return The read-only CommObjectTable address which can't be changed by KNX telegrams
     */
    word getCommObjectTableAddressStatic() const;

    /**
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

protected:
    // The method begin_BCU() is renamed during compilation to indicate the BCU type.
    // If you get a link error then the library's BCU_TYPE is different from your application's BCU_TYPE.
    void begin_BCU(int manufacturer, int deviceType, int version);
    /*
     * Special initialization for the BCU
     */
    virtual void _begin();

    /**
     * @brief Set or unset the programming mode of the bcu
     *
     * @param  new programming button state
     * @return true if successful, otherwise false
     */
    bool setProgrammingMode(bool newMode);

    Debouncer progButtonDebouncer; //!< The debouncer for the programming mode button.

private:
    word commObjectTableAddressStatic;       //!> The read-only CommObjectTable address which can't be changed by KNX telegrams
};


//
//  Inline functions
//
#if BCU_TYPE == BCU1_TYPE
    /**
     * Begin using the EIB bus coupling unit, and set the manufacturer-ID, device type,
     * program version and a optional read-only CommObjectTable address which
     * can't be changed by ETS/KNX telegrams
     *
     * @param manufacturer - the manufacturer ID (16 bit)
     * @param deviceType - the device type (16 bit)
     * @param version - the version of the application program (8 bit)
     */
inline void BcuBase::begin(int manufacturer, int deviceType, int version)
{
    begin_BCU(manufacturer, deviceType, version);
    commObjectTableAddressStatic = 0;
}
#else

/**
 * Begin using the EIB bus coupling unit, and set the manufacturer-ID, device type,
 * program version and a optional read-only CommObjectTable address which
 * can't be changed by ETS/KNX telegrams
 *
 * @param manufacturer - the manufacturer ID (16 bit)
 * @param deviceType - the device type (16 bit)
 * @param version - the version of the application program (8 bit)
 * @param readOnlyCommObjectTableAddress - optional (16bit), to set a read-only CommObjectTable address which can't be changed by KNX telegrams
 *                                         This is a workaround for ETS product databases, which send the "wrong" ComObjectTable address.
 *                                         Compare inside the extracted *.knxprod product database M-xxxx_x_xxxx-xx-xxxx.xml
 *                                         node <KNX/ManufacturerData/Manufacturer/ApplicationPrograms/ApplicationProgram/Static/ComObjectTable CodeSegment="M-xxxx_x-xxxx-xx-xxxx_xx-HHHH"
 *                                         HHHH = communication object table address in hexadecimal
 *                                         and
 *                                         node <KNX/ManufacturerData/Manufacturer/ApplicationPrograms/ApplicationProgram/Static/LoadProcedures/LdCtrlTaskSegment LsmIdx="3" Address="dddddd" />
 *                                         dddddd = communication object table address in decimal ETS wants us to use
 *                                         convert dddddd to hexadecimal => WWWW
 *                                         in case HHHH != WWWW ,
 *                                         use bcu.begin(MANUFACTURER, DEVICETYPE, APPVERSION, 0xHHHH) to set the correct read-only ComObjectTable address (HHHH)
 */
inline void BcuBase::begin(int manufacturer, int deviceType, int version, word readOnlyCommObjectTableAddress)
{
    begin_BCU(manufacturer, deviceType, version);
    commObjectTableAddressStatic = readOnlyCommObjectTableAddress;
    if ((commObjectTableAddressStatic != 0) && ( userEeprom.commsTabAddr != commObjectTableAddressStatic))
    {
        userEeprom.commsTabAddr = commObjectTableAddressStatic;
        userEeprom.modified();
    }
}
#endif

inline bool BcuBase::programmingMode() const
{
    return (userRam.status & BCU_STATUS_PROG) == BCU_STATUS_PROG;
}

inline bool BcuBase::applicationRunning() const
{
    if (!enabled)
        return false;

#if BCU_TYPE == BCU1_TYPE
    return ((userRam.status & (BCU_STATUS_PROG|BCU_STATUS_AL)) == BCU_STATUS_AL &&
        userRam.runState == 1 && userEeprom.runError == 0xff); // ETS sets the run error to 0 when programming
#else
    return userRam.runState == 1 &&
        userEeprom.loadState[OT_ADDR_TABLE] == LS_LOADED &&  // Address table object
        userEeprom.loadState[OT_ASSOC_TABLE] == LS_LOADED && // Association table object &
        userEeprom.loadState[OT_APPLICATION] == LS_LOADED;   // Application object. All three in State "Loaded"
#endif
}

inline unsigned short BcuBase::maskVersion()
{
    return MASK_VERSION;
}

inline word BcuBase::getCommObjectTableAddressStatic() const
{
    return commObjectTableAddressStatic;
}

#ifndef INSIDE_BCU_CPP
#   undef begin_BCU
#endif

#endif /*sblib_bcu_h*/
