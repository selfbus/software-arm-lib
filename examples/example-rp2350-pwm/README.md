# example-rp2350-pwm

Breathing-LED-Effekt auf der On-Board-LED (GP25) mittels Hardware-PWM.

Auf dem RP2350 wird PWM direkt über das Pico SDK `hardware_pwm` Modul
angesprochen, nicht über die sblib Timer-Klasse.

## Build

```bash
mkdir build && cd build
cmake -DPICO_SDK_PATH=/pfad/zum/pico-sdk ..
make
```
