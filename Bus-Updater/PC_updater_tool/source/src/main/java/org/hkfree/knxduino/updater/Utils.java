package org.hkfree.knxduino.updater;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.KNXIllegalArgumentException;

import java.io.File;
import java.net.InetAddress;
import java.net.UnknownHostException;

public class Utils {
    private static final Logger logger = LoggerFactory.getLogger(Utils.class.getName());
    public static int streamToInteger(byte[] stream, int offset) {
        //TODO this doesn't work for e.g. UDP_NOT_IMPLEMENTED
        // with stream[0]=0xFF and stream[1]=0xFF return is -1 but should be 0xFFFF
        // byte is signed in java -127...128, so that's our problem...
        return (stream[offset + 3] << 24) | (stream[offset + 2] << 16)
                | (stream[offset + 1] << 8) | stream[offset + 0];
    }

    public static void integerToStream(byte[] stream, int offset, long val) {
        stream[offset + 3] = (byte) (val >> 24);
        stream[offset + 2] = (byte) (val >> 16);
        stream[offset + 1] = (byte) (val >> 8);
        stream[offset + 0] = (byte) (val);
    }

    public static String byteArrayToHex(byte[] bytes) {
        if (bytes == null) {
            return "";
        }

        String txt = "";
        for (int i = 0; i < bytes.length; i++) {
            if (i != 0) {
                txt += ":";
            }
            txt += String.format("%02X", bytes[i]);
        }
        return txt;
    }
///\todo remove on release
/*
    public static void parseHost(final String host, final boolean local,
                                  final Map<String, Object> options) {
        try {
            if (local) {
                options.put("localhost", InetAddress.getByName(host));
            }
            else {
                options.put("host", InetAddress.getByName(host));
            }
        } catch (final UnknownHostException e) {
            throw new KNXIllegalArgumentException(
                    "failed to read host " + host, e);
        }
    }
*/
    public static InetAddress parseHost(final String host) {
        try {
            InetAddress res = InetAddress.getByName(host);
            logger.debug("Resolved {} with {}", host, res);
            return res;
        } catch (final UnknownHostException e) {
            throw new KNXIllegalArgumentException(
                    "failed to read host " + host, e);
        }
    }

    public static boolean fileExists(String fileName) {
        File f = new File(fileName);
        return (f.exists() && f.isFile());
    }
}
