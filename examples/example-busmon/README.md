Serial Bus Monitor Example
==========================

This is a bus monitor that outputs all received telegrams to the serial port.
The serial port is used with 115200 baud, 8 data bits, no parity, 1 stop bit.
Tx-pin is PIO1.6, Rx-pin is PIO1.7

In addition, one can send telegram on the bus via the serial port by sending
a sequence of bytes. The incoming data from the serial line should be in
following byte-format (with-out spaces):
CC aa bb cc dd ee
CC             the number of telegram-bytes to be send
aa,bb,cc,dd,ee the telegram data (without the checksum)
