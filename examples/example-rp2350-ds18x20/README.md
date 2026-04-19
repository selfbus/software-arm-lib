# example-rp2350-ds18x20

Liest OneWire DS18B20/DS18S20 Temperatursensoren alle 2 Sekunden und gibt die
Temperatur über die serielle Schnittstelle aus.

Demonstriert die sblib OneWire- und DS18x20-Treiber auf dem RP2350.

## Pinbelegung

| Funktion      | GPIO | Hinweis         |
|---------------|------|-----------------|
| OneWire Data  | GP16 | 4.7kΩ Pull-up   |
| Serial TX     | GP0  |                 |
| Serial RX     | GP1  |                 |
| LED           | GP25 |                 |

## Build

```bash
mkdir build && cd build
cmake -DPICO_SDK_PATH=/pfad/zum/pico-sdk ..
make
```
