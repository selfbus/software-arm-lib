# KNX Device Example for RP2350

Minimales Beispiel für ein KNX-Gerät auf dem RP2350 (Raspberry Pi Pico 2 / RP2354).

## Hardware

Benötigt wird ein KNX-Transceiver, der an den RP2350 angeschlossen ist:

| Funktion | GPIO | Beschreibung |
|----------|------|-------------|
| KNX TX   | GP2  | Aktiv-Low Puls zum Bus (über Transceiver) |
| KNX RX   | GP3  | Bus-Empfang (Falling Edge = 0-Bit) |
| Prog LED | GP25 | On-Board LED (Pico) |

### Transceiver-Optionen

- **NCN5120/NCN5130**: Dedizierte KNX-Transceiver von onsemi (TPUART-kompatibel)  
  → Bei Verwendung kann statt `KnxBusPio` auch `KnxBusTpuart` verwendet werden
- **Siemens TPUART / TPUART2**: Original KNX-Busankoppler-IC (UART-basiert)  
  → Verwendung mit `KnxBusTpuart`
- **STKNX (STMicroelectronics)**: KNX-Transceiver-PHY mit Bitstream-Interface  
  → Verwendung mit `KnxBusPio`
- **Selfbus Interface**: Diskrete Transceiver-Schaltung des Selfbus-Projekts  
  → Verwendung mit `KnxBusPio`

## Build

```bash
export PICO_SDK_PATH=/pfad/zum/pico-sdk
mkdir build && cd build
cmake ..
make
```

Die `.uf2`-Datei kann dann per Drag & Drop auf den Pico im BOOTSEL-Modus kopiert werden.

## Architektur

```
┌─────────────────────────────────┐
│         Applikation             │
│    (setup/loop, ComObjects)     │
├─────────────────────────────────┤
│     KNX Protokoll-Stack         │
│  (BcuBase, TLayer4, ComObj)     │
├─────────────────────────────────┤
│      KnxBusInterface            │  ← Abstrakte Schnittstelle
├─────────┬───────────┬───────────┤
│ PIO Bus │  TPUART   │ Legacy    │  ← Austauschbare Implementierungen
│(RP2350  │(NCN51xx   │ (LPC11xx) │
│ /STKNX) | /TPUART)  │           │
└─────────┴───────────┴───────────┘
```
