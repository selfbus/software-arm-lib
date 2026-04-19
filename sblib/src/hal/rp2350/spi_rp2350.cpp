/*
 *  spi_rp2350.cpp - SPI implementation for RP2350 using Pico SDK.
 *
 *  Wraps the Pico SDK SPI hardware interface to provide the same
 *  sblib SPI class API that the LPC11xx version offers.
 *
 *  Copyright (c) 2026 Selfbus project. Licensed under GPLv3.
 */

#ifdef __SBLIB_TARGET_RP2350__

#include <sblib/spi.h>

#include "hardware/spi.h"
#include "hardware/gpio.h"

// Default SPI pin assignments (overridable via project defines)
#ifndef SPI0_SCK_PIN
#define SPI0_SCK_PIN   2
#endif
#ifndef SPI0_MOSI_PIN
#define SPI0_MOSI_PIN  3
#endif
#ifndef SPI0_MISO_PIN
#define SPI0_MISO_PIN  4
#endif

#ifndef SPI1_SCK_PIN
#define SPI1_SCK_PIN   10
#endif
#ifndef SPI1_MOSI_PIN
#define SPI1_MOSI_PIN  11
#endif
#ifndef SPI1_MISO_PIN
#define SPI1_MISO_PIN  12
#endif

// Default SPI base clock (1 MHz)
#define SPI_DEFAULT_BAUDRATE 1000000

static inline spi_inst_t* getSpiInst(int portNum)
{
    return portNum == 0 ? spi0 : spi1;
}

static spi_cpol_t modeToPolarity(int mode)
{
    return (mode & SPI_CPOL_HIGH) ? SPI_CPOL_1 : SPI_CPOL_0;
}

static spi_cpha_t modeToPhase(int mode)
{
    return (mode & SPI_CPHASE_FALL) ? SPI_CPHA_1 : SPI_CPHA_0;
}

SPI::SPI(int spiPort, int mode)
    : port(nullptr)
    , spiPortNum(spiPort)
    , clockDiv(1)
    , sndData(nullptr)
    , recData(nullptr)
    , sndCount(0)
    , recCount(0)
    , errors(0)
    , finished(true)
{
    spi_inst_t* spi = getSpiInst(spiPort);
    port = spi;

    spi_init(spi, SPI_DEFAULT_BAUDRATE);
    spi_set_format(spi, 8, modeToPolarity(mode), modeToPhase(mode), SPI_MSB_FIRST);

    // Configure GPIO pins for SPI function
    if (spiPort == 0)
    {
        gpio_set_function(SPI0_SCK_PIN, GPIO_FUNC_SPI);
        gpio_set_function(SPI0_MOSI_PIN, GPIO_FUNC_SPI);
        gpio_set_function(SPI0_MISO_PIN, GPIO_FUNC_SPI);
    }
    else
    {
        gpio_set_function(SPI1_SCK_PIN, GPIO_FUNC_SPI);
        gpio_set_function(SPI1_MOSI_PIN, GPIO_FUNC_SPI);
        gpio_set_function(SPI1_MISO_PIN, GPIO_FUNC_SPI);
    }
}

void SPI::setClockDivider(int div)
{
    clockDiv = div;
    spi_inst_t* spi = static_cast<spi_inst_t*>(port);
    uint baudrate = SPI_DEFAULT_BAUDRATE / (div > 0 ? div : 1);
    spi_set_baudrate(spi, baudrate);
}

void SPI::setDataSize(SpiDataSize dataSize)
{
    spi_inst_t* spi = static_cast<spi_inst_t*>(port);
    spi_set_format(spi, static_cast<uint>(dataSize), SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

void SPI::begin()
{
    // SPI is already initialized in the constructor on RP2350
}

void SPI::end()
{
    spi_inst_t* spi = static_cast<spi_inst_t*>(port);
    spi_deinit(spi);
}

int SPI::transfer(int val, SpiTransferMode transferMode)
{
    spi_inst_t* spi = static_cast<spi_inst_t*>(port);
    uint8_t txBuf = static_cast<uint8_t>(val);
    uint8_t rxBuf = 0;
    spi_write_read_blocking(spi, &txBuf, &rxBuf, 1);
    return rxBuf;
}

void SPI::transferBlock(uint16_t* sndData, int bytes, uint16_t* recData, bool asynchron)
{
    (void)asynchron; // RP2350: always blocking

    spi_inst_t* spi = static_cast<spi_inst_t*>(port);
    this->errors = 0;
    this->finished = false;

    if (recData)
    {
        // Full duplex: write and read simultaneously
        // Pico SDK expects uint8_t buffers, but sblib uses uint16_t
        // For 8-bit mode, low byte of each uint16_t is used
        for (int i = 0; i < bytes; i++)
        {
            uint8_t tx = static_cast<uint8_t>(sndData[i]);
            uint8_t rx = 0;
            spi_write_read_blocking(spi, &tx, &rx, 1);
            recData[i] = rx;
        }
    }
    else
    {
        // Write only
        for (int i = 0; i < bytes; i++)
        {
            uint8_t tx = static_cast<uint8_t>(sndData[i]);
            spi_write_blocking(spi, &tx, 1);
        }
    }

    this->finished = true;
}

void SPI::continueBlockTransfer(void)
{
    // Not needed on RP2350 (all transfers are blocking)
}

void SPI::finalizeBlockTransfer(void)
{
    // Not needed on RP2350 (all transfers are blocking)
}

#endif // __SBLIB_TARGET_RP2350__
