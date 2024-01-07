package org.selfbus.updater;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import io.calimero.KNXException;
import io.calimero.KNXIllegalArgumentException;
import io.calimero.knxnetip.TcpConnection;

import java.io.File;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.Map;
import java.util.zip.CRC32;

/**
 * Basic utilities usable for the application
 */
public class Utils {
    private static final Logger logger = LoggerFactory.getLogger(Utils.class.getName());
    public static final String PROGRESS_MARKER = "."; //!< symbol to print progress to console


    public static long streamToLong(byte[] stream, int offset) {
        return ((stream[offset] & 0xFF)) |
               ((stream[offset + 1] & 0xFF) <<  8) |
               ((stream[offset + 2] & 0xFF) << 16) |
               ((long) (stream[offset + 3] & 0xFF) << 24);
    }

    public static short streamToShort(byte[] stream, int offset) {
        return (short) ((stream[offset] & 0xFF) | ((stream[offset + 1] & 0xFF) << 8));
    }
    public static void longToStream(byte[] stream, int offset, long val) {
        stream[offset + 3] = (byte) (val >> 24);
        stream[offset + 2] = (byte) (val >> 16);
        stream[offset + 1] = (byte) (val >> 8);
        stream[offset] = (byte) (val);
    }
    public static void shortToStream(byte[] stream, int offset, short val) {
        stream[offset + 1] = (byte) (val >> 8);
        stream[offset] = (byte) (val);
    }

    public static String byteArrayToHex(byte[] bytes) {
        if (bytes == null) {
            return "";
        }

        StringBuilder txt = new StringBuilder();
        for (int i = 0; i < bytes.length; i++) {
            if (i != 0) {
                txt.append(":");
            }
            txt.append(String.format("%02X", bytes[i]));
        }
        return txt.toString();
    }

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

    public static final int DEFAULT_SHORTENER_THRESHOLD = 4;
    public static final String SHORTENER_BACKSLASH_REGEX = "\\\\";
    public static final String SHORTENER_SLASH_REGEX = "/";
    public static final String SHORTENER_BACKSLASH = "\\";
    public static final String SHORTENER_SLASH = "/";
    public static final String SHORTENER_ELLIPSE = "...";

    /**
     * Shortens the path to a maximum of 4 path elements including filename.
            *
            * @param path the path to the file (relative or absolute)
     * @return shortened path
     */
    @SuppressWarnings("unused")
    public static String shortenPath(final String path) {
        return shortenPath(path, DEFAULT_SHORTENER_THRESHOLD);
    }

    /**
     * Shortens the path based on the given maximum number of path elements. E.g.,
     * "C:/1/2/test.txt" returns "C:/.../2/test.txt" if threshold is 2.
     *
     * @param path the path to the file (relative or absolute)
     * @param threshold the number of directories including filename to keep
     * @return shortened path
     */
    public static String shortenPath(final String path, final int threshold) {
        String regex = SHORTENER_BACKSLASH_REGEX;
        String sep = SHORTENER_BACKSLASH;

        if (path.contains("/")) {
            regex = SHORTENER_SLASH_REGEX;
            sep = SHORTENER_SLASH;
        }

        String[] pathParts = path.split(regex);

        int elem = pathParts.length;
        if (elem <= threshold) {
            // nothing to shorten
            return path;
        }

        final StringBuilder sb = new StringBuilder();
        int index = 0;
        // traverse path from back
        for (; index < threshold; index++) {
            sb.insert(0, pathParts[pathParts.length - index - 1]).insert(0, sep);
        }

        // path shortener ...
        sb.insert(0, SHORTENER_ELLIPSE).insert(0, sep);

        // drive or protocol
        final int pos2dots = path.indexOf(":");
        if (pos2dots > 0) {
            // case c:\ c:/ etc.
            sb.insert(0,path.substring(0, pos2dots + 1));
            // case http:// ftp:// etc.
            if (path.indexOf(":/") > 0 && pathParts[0].length() > 2) {
                sb.insert(path.indexOf(":/") + 2,SHORTENER_SLASH);
            }
        } else {
            String tmp = path.substring(0, 2);
            final boolean isUNC = tmp.equals(SHORTENER_BACKSLASH_REGEX);
            if (isUNC) {
                sb.insert(0, SHORTENER_BACKSLASH);
            }
        }

        return sb.toString();
    }

    public static int crc32Value(byte[] buffer) {
        CRC32 crc32Block = new CRC32();
        crc32Block.update(buffer, 0, buffer.length);
        return (int) crc32Block.getValue();
    }

    private static final Map<InetSocketAddress, TcpConnection> tcpConnectionPool = new HashMap<>();
    private static boolean registeredTcpShutdownHook;

    static synchronized TcpConnection tcpConnection(final InetSocketAddress local, final InetSocketAddress server)
            throws KNXException {
        if (!registeredTcpShutdownHook) {
            Runtime.getRuntime().addShutdownHook(new Thread(Utils::closeTcpConnections));
            registeredTcpShutdownHook = true;
        }

        var connection = tcpConnectionPool.get(server);
        if (connection == null || !connection.isConnected()) {
            connection = TcpConnection.newTcpConnection(local, server);
            tcpConnectionPool.put(server, connection);
        }
        return connection;
    }

    private static void closeTcpConnections() {
        final var connections = tcpConnectionPool.values().toArray(TcpConnection[]::new);
        for (final var c : connections) {
            c.close();
        }
    }
}
