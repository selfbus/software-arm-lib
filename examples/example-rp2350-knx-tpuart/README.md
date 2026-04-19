# example-rp2350-knx-tpuart

Minimales KNX-Gerät auf dem RP2350 mit einem TPUART/NCN5120-Transceiver.

Demonstriert die Verwendung von `KnxBusTpuart` mit einer plattformspezifischen
UART-Implementierung (`PicoTpuartUart`). Unterstützte Transceiver:

- Siemens TP-UART / TP-UART 2
- onsemi NCN5120 / NCN5121 / NCN5130

## Pinbelegung

| Funktion      | GPIO |
|---------------|------|
| TPUART TX     | GP8  |
| TPUART RX     | GP9  |
| Prog-LED      | GP25 |

UART-Protokoll: 19200 Baud, 8E1 (TPUART-Standard).

## Build

```bash
mkdir build && cd build
cmake -DPICO_SDK_PATH=/pfad/zum/pico-sdk ..
make
```
