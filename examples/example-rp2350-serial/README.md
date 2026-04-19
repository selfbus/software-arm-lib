# example-rp2350-serial

Demonstriert die sblib `Serial`-Klasse auf dem RP2350.

UART0 auf GP0 (TX) und GP1 (RX), 115200 Baud, 8N1.
Gibt jede Sekunde einen Zähler aus und gibt empfangene Zeichen als Echo zurück.

## Pinbelegung

| Funktion | GPIO |
|----------|------|
| TX       | GP0  |
| RX       | GP1  |

## Build

```bash
mkdir build && cd build
cmake -DPICO_SDK_PATH=/pfad/zum/pico-sdk ..
make
```
