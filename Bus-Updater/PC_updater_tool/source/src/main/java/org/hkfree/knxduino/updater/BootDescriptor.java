package org.hkfree.knxduino.updater;

public class BootDescriptor {
    private static final long INVALID_APP_VERSION_ADDRESS = 0xFFFFFFFF;
    private final long  startAddress;
    private final long  endAddress;
    private final int  crc32;
    private final long appVersionAddress;

    public BootDescriptor(long startAddress, long endAddress, int crc32, long appVersionAddress) {
        this.startAddress = startAddress;
        this.endAddress = endAddress;
        this.crc32 = crc32;
        this.appVersionAddress = appVersionAddress;
    }


    public BootDescriptor(byte[] parse) {
        this.startAddress = (parse[0] & 0xFF) +
                            ((parse[1] & 0xFF) << 8) +
                            ((parse[2] & 0xFF) << 16) +
                            ((long)(parse[3] & 0xFF) << 24);
        this.endAddress = (parse[4] & 0xFF) +
                          ((parse[5] & 0xFF) << 8) +
                          ((parse[6] & 0xFF) << 16) +
                          ((long)(parse[7] & 0xFF) << 24);
        this.crc32 = (parse[8] & 0xFF) +
                     ((parse[9] & 0xFF) << 8) +
                     ((parse[10] & 0xFF) << 16) +
                     ((parse[11] & 0xFF) << 24);
        this.appVersionAddress = INVALID_APP_VERSION_ADDRESS;
    }

    public long startAddress() {
        return this.startAddress;
    }

    public long endAddress() {
        return this.endAddress;
    }

    public long appVersionAddress() {
        return this.appVersionAddress;
    }

    public int crc32() {
        return this.crc32;
    }
    public long length() {
        return this.endAddress - this.startAddress;
    }

    public String toString() {
        String res;
        /*
        res = String.format("start: 0x%04X, end: 0x%04X, size: 0x%04X, crc32: 0x%04X",
                            startAddress, endAddress, length(), crc32);
         */
        res = String.format("start: 0x%s, end: 0x%s, size: 0x%s, crc32: 0x%s",
                Long.toHexString(startAddress), Long.toHexString(endAddress), Long.toHexString(length()),Integer.toHexString(crc32));
        if (appVersionAddress != INVALID_APP_VERSION_ADDRESS)
        {
            res += String.format(", APP_VERSION pointer: 0x%s", Long.toHexString(appVersionAddress));
        }
        return res;
    }

    public byte[] toStream() {
        byte[] stream = new byte[16];
        Utils.integerToStream(stream, 0, startAddress);
        Utils.integerToStream(stream, 4, endAddress);
        Utils.integerToStream(stream, 8, crc32);
        Utils.integerToStream(stream, 12, appVersionAddress);
        return stream;
    }
}
