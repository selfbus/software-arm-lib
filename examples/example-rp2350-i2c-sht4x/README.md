# example-rp2350-i2c-sht4x

Liest den SHT4x Temperatur-/Feuchtigkeitssensor alle 2 Sekunden via I2C und
gibt die Messwerte über die serielle Schnittstelle aus.

Demonstriert, dass die sblib-I2C-Sensortreiber dank der `Chip_I2C_*`-HAL-Schicht
unverändert auf dem RP2350 funktionieren.

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
