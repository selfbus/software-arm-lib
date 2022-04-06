Serial Bus Monitor 2 Example
============================

This is a bus monitor that outputs all received KNX-Bus bytes to the serial port.\
It uses the debug macros of the sblib for that.\
The serial port is used with 576,000 baud, 8 data bits, no parity, 1 stop bit.\
Serial pins for a 4TE Controller: Tx-pin PIO3.0, Rx-pin is PIO3.1\
Serial pins for a TS_ARM Controller: Tx-pin PIO2.8, Rx-pin is PIO2.7\

\#define BUSMONITOR and \#define DUMP_TELEGRAMS must be "enabled" in sblib/inc/libconfig.h