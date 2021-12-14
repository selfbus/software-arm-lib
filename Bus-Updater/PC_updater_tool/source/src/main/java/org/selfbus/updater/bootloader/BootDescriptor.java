package org.selfbus.updater.bootloader;

import org.selfbus.updater.UpdaterException;
import org.selfbus.updater.Utils;

/**
 *  Holds Application Boot Block Descriptor the MCU's bootloader
 *  see software-arm-lib/Bus-Updater/inc/boot_descriptor_block.h for more information
 */
public class BootDescriptor {
    private static final long INVALID_APP_VERSION_ADDRESS = 0xFFFFFFFF;
    private long  startAddress;
    private long  endAddress;
    private int  crc32;
    private long appVersionAddress;
    private boolean valid;

    private BootDescriptor() {}

    public BootDescriptor(long startAddress, long endAddress, int crc32, long appVersionAddress) throws UpdaterException {
        if (startAddress > endAddress) {
            throw new UpdaterException("startAddress beyond endAddress");
        }
        this.startAddress = startAddress;
        this.endAddress = endAddress;
        this.crc32 = crc32;
        this.appVersionAddress = appVersionAddress;

        valid = (this.startAddress != 0xFFFFFFFFL);
    }


    public static BootDescriptor fromArray(byte[] parse) throws UpdaterException {
        long startAddr = (parse[0] & 0xFF) +
                            ((parse[1] & 0xFF) << 8) +
                            ((parse[2] & 0xFF) << 16) +
                            ((long)(parse[3] & 0xFF) << 24);
        long endAddr = (parse[4] & 0xFF) +
                          ((parse[5] & 0xFF) << 8) +
                          ((parse[6] & 0xFF) << 16) +
                          ((long)(parse[7] & 0xFF) << 24);
        int crc32 = (parse[8] & 0xFF) +
                     ((parse[9] & 0xFF) << 8) +
                     ((parse[10] & 0xFF) << 16) +
                     ((parse[11] & 0xFF) << 24);
        return new BootDescriptor(startAddr, endAddr, crc32, INVALID_APP_VERSION_ADDRESS);
    }

    public long startAddress() {
        return startAddress;
    }

    public long endAddress() {
        return endAddress;
    }

    public long appVersionAddress() {
        return appVersionAddress;
    }

    public boolean valid() {
        return valid;
    }

    public int crc32() {
        return this.crc32;
    }
    public long length() {
        return endAddress() - startAddress() + 1;
    }

    public String toString() {
        String res;
        res = String.format("valid: %b, start: 0x%04X, end: 0x%04X, size: 0x%04X, crc32: 0x%04X",
                            valid, startAddress, endAddress, length(), crc32);
        if (appVersionAddress != INVALID_APP_VERSION_ADDRESS)
        {
            res += String.format(", APP_VERSION pointer: 0x%04X", appVersionAddress);
        }
        return res;
    }

    public byte[] toStream() {
        byte[] stream = new byte[16];
        Utils.longToStream(stream, 0, startAddress());
        Utils.longToStream(stream, 4, endAddress());
        Utils.longToStream(stream, 8, crc32());
        Utils.longToStream(stream, 12, appVersionAddress());
        return stream;
    }
}
