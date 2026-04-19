# example-rp2350-i2c-bh1750

Liest den BH1750 Lichtsensor alle 2 Sekunden via I2C und gibt den Lux-Wert
über die serielle Schnittstelle aus.

## Pinbelegung

| Funktion   | GPIO |
|------------|------|
| I2C SDA    | GP4  |
| I2C SCL    | GP5  |
| Serial TX  | GP0  |
| Serial RX  | GP1  |
| LED        | GP25 |

## Build

```bash
mkdir build && cd build
cmake -DPICO_SDK_PATH=/pfad/zum/pico-sdk ..
make
```
