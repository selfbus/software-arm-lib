i2c example
===========

An example application for a graphical LCD.
We use the EA DOGS 102, a 102x64 monochrome graphical LCD.

We use SPI port 0 in this example.

Connect the ARM in this way to the EA-DOGS:

PIO0_2:  SSEL0 -> Display CS0 "chip select"
PIO0_9:  MOSI0 -> Display SDA "data in"
PIO2_11: SCK0  -> Display SCK "clock"
PIO0_8:        -> Display CD  "command/data"

