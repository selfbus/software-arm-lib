Processor UID (GUID) and serial example
=======================================

A simple application which:
- reads the UID (GUID) of the LPCxx processor
- calculates a hashed serial for the KNX bus
- blinks the program led at ~2Hz
- sends UID, shorted UID (for bus-updater use) and KNX-serial to the serial port

Connect a terminal program to the ARM's serial port.<br />
The connection setting ist 19200 8N1 (19200 baud, 8 data bits, no parity, 1 stop bit).<br />
Default Tx-pin is PIO1.7 and Rx-pin is PIO1.6.<br />
You can change the serial port by commenting/uncommenting #define PIN_SERIAL_RX and #define PIN_SERIAL_TX.

- links against BCU1 version of the sblib library