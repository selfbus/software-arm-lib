/**
 *  com_objects_debug.cpp - KNX Communication objects debugging helper.
 *
 */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#include <sblib/eib/com_objects_debug.h>
#include <sblib/eib/types.h>

void printComObjectConfig(uint8_t config)
{
    DB_COM_OBJ(
        // C - communicate
        if ((config & 0b00000100) == (0b00000100))
        {
            serial.print("c");
        }
        else
        {
            serial.print(" ");
        }

        // R - read
        if ((config & 0b00001000) == (0b00001000))
        {
            serial.print("r");
        }
        else
        {
            serial.print(" ");
        }

        // W - write
        if ((config & 0b00010000) == (0b00010000))
        {
            serial.print("w");
        }
        else
        {
            serial.print(" ");
        }

        // T - transfer
        if ((config & 0b01000000) == (0b01000000))
        {
            serial.print("t");
        }
        else
        {
            serial.print(" ");
        }

        // U - update
        if ((config & 0b10000000) == (0b10000000))
        {
            serial.print("u");
        }
        else
        {
            serial.print(" ");
        }

        // Segment Selector Type
        serial.print(" s");
        if ((config & 0b00100000) == (0b00100000))
        {
            serial.print("1"); // segment = 0x100
        }
        else
        {
            serial.print("0"); // segment = 0x00
        }

        serial.print(" ");
        switch (config & 0b00000011)
        {
            case 0b11:
                serial.print("low ");
                break;
            case 0b01:
                serial.print("high");
                break;
            case 0b10:
                serial.print("alm ");
                break;
            case 0b00:
                serial.print("sys ");
                break;
            default:
                // this should never happen
                serial.print("unknown");
                break;
        }
    );
}

void printComObjectType(uint8_t type)
{
    DB_COM_OBJ(
        if ((type >= BIT_1) && (type <= BIT_7))
        {
            serial.print(type + 1, DEC, 1);
            serial.print(" bit");
            if (type > 0)
            {
                serial.print("s"); // bits, if more then one
            }
        }
        else
        {
            switch(type)
            {
                case BYTE_1:
                    serial.print("1 byte");
                    break;

                case BYTE_2:
                    serial.print("2 bytes");
                    break;

                case BYTE_3:
                    serial.print("3 bytes");
                    break;

                case BYTE_4:
                    serial.print("4 bytes");
                    break;

                case DATA_6:
                    serial.print("6 bytes");
                    break;

                case DATA_8:
                    serial.print("8 bytes");
                    break;

                case DATA_10:
                    serial.print("10 bytes");
                    break;

                case MAXDATA:
                    serial.print("14 bytes");
                    break;

                case VARDATA:
                    serial.print("1-14 bytes");
                    break;

                default:
                    serial.print("unknown (0x", type, HEX, 2);
                    serial.print(")");
                    break;
            }
        }

    );
}
