# example-rp2350-spi

Sendet jede Sekunde ein inkrementierendes Byte über SPI0 und toggelt dabei
die On-Board-LED.

Demonstriert die sblib `SPI`-Klasse auf dem RP2350.

## Pinbelegung

| Funktion | GPIO |
|----------|------|
| SPI SCK  | GP2  |
| SPI MOSI | GP3  |
| SPI MISO | GP4  |
| LED      | GP25 |

## Build

```bash
mkdir build && cd build
cmake -DPICO_SDK_PATH=/pfad/zum/pico-sdk ..
make
```
