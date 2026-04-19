# example-rp2350-blink

Blinkt die On-Board-LED (GP25) des Raspberry Pi Pico / Pico 2 im 250 ms Takt.

Demonstriert die sblib-GPIO-Funktionen `pinMode()`, `digitalWrite()`, `digitalRead()`
und `delay()` auf dem RP2350.

## Build

```bash
mkdir build && cd build
cmake -DPICO_SDK_PATH=/pfad/zum/pico-sdk ..
make
```

Dann die erzeugte `.uf2`-Datei per Drag & Drop auf den Pico flashen.
