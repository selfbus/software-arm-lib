/*
 *  i2c_rp2350.cpp - I2C driver for RP2350 implementing the Chip_I2C_* API.
 *
 *  This wraps the Pico SDK I2C functions to provide the same API used by
 *  all sblib sensor drivers (BH1750, DS3231, SGP4x, SHT2x, SHT4x, etc.).
 *
 *  Default pins: GP4 (SDA), GP5 (SCL) on I2C0.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#if defined(__SBLIB_TARGET_RP2350__)

#include <sblib/i2c.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <string.h>

// Default I2C pins for RP2350
#if !defined(I2C_SDA_PIN)
#   define I2C_SDA_PIN 4
#endif
#if !defined(I2C_SCL_PIN)
#   define I2C_SCL_PIN 5
#endif

static i2c_inst_t* rp_i2c = i2c0;
static uint32_t i2c_clock_rate = 100000;

bool i2c_initialized = false;

void i2c_lpcopen_init()
{
    Chip_I2C_Init(I2C0);
    Chip_I2C_SetClockRate(I2C0, 100000);
}

void Chip_I2C_Init(I2C_ID_T id)
{
    (void)id;
    i2c_init(rp_i2c, i2c_clock_rate);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    i2c_initialized = true;
}

void Chip_I2C_DeInit(I2C_ID_T id)
{
    (void)id;
    i2c_deinit(rp_i2c);
    i2c_initialized = false;
}

void Chip_I2C_SetClockRate(I2C_ID_T id, uint32_t clockrate)
{
    (void)id;
    i2c_clock_rate = clockrate;
    if (i2c_initialized)
        i2c_set_baudrate(rp_i2c, clockrate);
}

uint32_t Chip_I2C_GetClockRate(I2C_ID_T id)
{
    (void)id;
    return i2c_clock_rate;
}

int Chip_I2C_MasterTransfer(I2C_ID_T id, I2C_XFER_T *xfer)
{
    (void)id;
    xfer->status = I2C_STATUS_BUSY;

    if (xfer->txSz > 0 && xfer->rxSz > 0)
    {
        // Write then read (repeated start)
        int ret = i2c_write_blocking(rp_i2c, xfer->slaveAddr, xfer->txBuff, (size_t)xfer->txSz, true);
        if (ret == PICO_ERROR_GENERIC)
        {
            xfer->status = I2C_STATUS_NAK;
            return (int)I2C_STATUS_NAK;
        }
        ret = i2c_read_blocking(rp_i2c, xfer->slaveAddr, xfer->rxBuff, (size_t)xfer->rxSz, false);
        if (ret == PICO_ERROR_GENERIC)
        {
            xfer->status = I2C_STATUS_NAK;
            return (int)I2C_STATUS_NAK;
        }
    }
    else if (xfer->txSz > 0)
    {
        int ret = i2c_write_blocking(rp_i2c, xfer->slaveAddr, xfer->txBuff, (size_t)xfer->txSz, false);
        if (ret == PICO_ERROR_GENERIC)
        {
            xfer->status = I2C_STATUS_NAK;
            return (int)I2C_STATUS_NAK;
        }
    }
    else if (xfer->rxSz > 0)
    {
        int ret = i2c_read_blocking(rp_i2c, xfer->slaveAddr, xfer->rxBuff, (size_t)xfer->rxSz, false);
        if (ret == PICO_ERROR_GENERIC)
        {
            xfer->status = I2C_STATUS_NAK;
            return (int)I2C_STATUS_NAK;
        }
    }

    xfer->status = I2C_STATUS_DONE;
    return (int)I2C_STATUS_DONE;
}

int Chip_I2C_MasterSend(I2C_ID_T id, uint8_t slaveAddr, const uint8_t *buff, uint8_t len)
{
    (void)id;
    int ret = i2c_write_blocking(rp_i2c, slaveAddr, buff, (size_t)len, false);
    return (ret == PICO_ERROR_GENERIC) ? (int)I2C_STATUS_NAK : len;
}

int Chip_I2C_MasterRead(I2C_ID_T id, uint8_t slaveAddr, uint8_t *buff, int len)
{
    (void)id;
    int ret = i2c_read_blocking(rp_i2c, slaveAddr, buff, (size_t)len, false);
    return (ret == PICO_ERROR_GENERIC) ? (int)I2C_STATUS_NAK : len;
}

int Chip_I2C_MasterCmdRead(I2C_ID_T id, uint8_t slaveAddr, uint8_t cmd, uint8_t *buff, int len)
{
    (void)id;
    int ret = i2c_write_blocking(rp_i2c, slaveAddr, &cmd, 1, true);
    if (ret == PICO_ERROR_GENERIC)
        return (int)I2C_STATUS_NAK;
    ret = i2c_read_blocking(rp_i2c, slaveAddr, buff, (size_t)len, false);
    return (ret == PICO_ERROR_GENERIC) ? (int)I2C_STATUS_NAK : len;
}

int Chip_I2C_MasterWriteRead(I2C_ID_T id, uint8_t slaveAddr, uint8_t *cmd, uint8_t *buff, int txlen, int rxlen)
{
    (void)id;
    int ret = i2c_write_blocking(rp_i2c, slaveAddr, cmd, (size_t)txlen, true);
    if (ret == PICO_ERROR_GENERIC)
        return (int)I2C_STATUS_NAK;
    ret = i2c_read_blocking(rp_i2c, slaveAddr, buff, (size_t)rxlen, false);
    return (ret == PICO_ERROR_GENERIC) ? (int)I2C_STATUS_NAK : rxlen;
}

// Event handler stubs — not needed for blocking I2C on RP2350
static I2C_EVENTHANDLER_T i2c_event_handler = nullptr;

int Chip_I2C_SetMasterEventHandler(I2C_ID_T id, I2C_EVENTHANDLER_T event)
{
    (void)id;
    i2c_event_handler = event;
    return 0;
}

I2C_EVENTHANDLER_T Chip_I2C_GetMasterEventHandler(I2C_ID_T id)
{
    (void)id;
    return i2c_event_handler;
}

void Chip_I2C_EventHandlerPolling(I2C_ID_T id, I2C_EVENT_T event)
{
    (void)id;
    (void)event;
}

void Chip_I2C_EventHandler(I2C_ID_T id, I2C_EVENT_T event)
{
    (void)id;
    (void)event;
}

void Chip_I2C_MasterStateHandler(I2C_ID_T id)
{
    (void)id;
}

void Chip_I2C_Disable(I2C_ID_T id)
{
    Chip_I2C_DeInit(id);
}

int Chip_I2C_IsMasterActive(I2C_ID_T id)
{
    (void)id;
    return 0; // Blocking mode — never active between calls
}

void Chip_I2C_SlaveSetup(I2C_ID_T id, I2C_SLAVE_ID slaveIndex,
    I2C_XFER_T *xfer, I2C_EVENTHANDLER_T event, uint8_t addrMask)
{
    (void)id; (void)slaveIndex; (void)xfer; (void)event; (void)addrMask;
}

void Chip_I2C_SlaveStateHandler(I2C_ID_T id)
{
    (void)id;
}

int Chip_I2C_IsStateChanged(I2C_ID_T id)
{
    (void)id;
    return 0;
}

#endif // __SBLIB_TARGET_RP2350__
