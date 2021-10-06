PWM blink example
-------------------

This example application flashes a LED on digital pin PIO2.6
using the PWM output of the 16bit timer #1.

To test, connect: PIO2.6 --[ 470 ohm resistor ]---[ LED ]--- GND

The LED to blink has to be connected to pin PIN_INFO (PIO2.6)
We cannot blink the LED on the LPCxpresso board with PWM.
