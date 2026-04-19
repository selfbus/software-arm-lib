Selfbus sblib library usage examples
====================================

## LPC11xx Examples

Die Original-Beispiele sind für den NXP LPC11xx (ARM Cortex-M0) geschrieben
und werden mit MCUXpresso / LPCXpresso gebaut.

## RP2350 Examples

Die folgenden Beispiele sind für den Raspberry Pi RP2350 (Pico 2) portiert
und verwenden das Pico SDK. Build-Anleitung:

```bash
cd examples/example-rp2350-<name>
mkdir build && cd build
cmake -DPICO_SDK_PATH=/pfad/zum/pico-sdk ..
make
```

| Beispiel                      | Beschreibung                              |
|-------------------------------|-------------------------------------------|
| example-rp2350-blink          | LED blinken (GPIO)                        |
| example-rp2350-serial         | Serielle Ausgabe (UART0)                  |
| example-rp2350-timer-int      | Timer-Interrupt (LED toggle)              |
| example-rp2350-analog         | ADC lesen (GP26)                          |
| example-rp2350-spi            | SPI Ausgabe                               |
| example-rp2350-pwm            | PWM Breathing LED                         |
| example-rp2350-i2c-sht4x     | I2C SHT4x Temperatur/Feuchte             |
| example-rp2350-i2c-bh1750    | I2C BH1750 Lichtsensor                   |
| example-rp2350-ds18x20       | OneWire DS18B20 Temperatur                |
| example-rp2350-knx           | KNX via PIO (diskrete Schaltung/STKNX)    |
| example-rp2350-knx-tpuart    | KNX via TPUART/NCN5120                    |
