# example-rp2350-timer-int

Blinkt die On-Board-LED (GP25) mit einem Timer-Interrupt alle 500 ms.

Demonstriert die sblib `Timer`-Klassen-Emulation für RP2350.
Verwendet das gleiche Muster wie das LPC11xx-Beispiel `example-int-blink`:

```cpp
timer32_0.begin();
timer32_0.prescaler((SystemCoreClock / 1000) - 1);
timer32_0.matchMode(MAT1, RESET | INTERRUPT);
timer32_0.match(MAT1, 500);
timer32_0.start();
```

## Build

```bash
mkdir build && cd build
cmake -DPICO_SDK_PATH=/pfad/zum/pico-sdk ..
make
```
