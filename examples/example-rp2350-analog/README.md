# example-rp2350-analog

Liest ADC-Kanal 0 (GP26) alle 500 ms und gibt den Wert (0–1023) über die
serielle Schnittstelle aus.

Demonstriert die sblib-ADC-Funktionen `analogBegin()` und `analogRead()` auf
dem RP2350. Der 12-Bit-ADC des RP2350 wird intern auf 10 Bit skaliert, um
API-kompatibel zur LPC11xx-Version zu bleiben.

## Pinbelegung

| Funktion   | GPIO |
|------------|------|
| ADC0       | GP26 |
| Serial TX  | GP0  |
| Serial RX  | GP1  |

## Build

```bash
mkdir build && cd build
cmake -DPICO_SDK_PATH=/pfad/zum/pico-sdk ..
make
```
