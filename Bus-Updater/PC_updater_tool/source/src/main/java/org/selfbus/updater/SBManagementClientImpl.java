package org.selfbus.updater;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.*;
import tuwien.auto.calimero.link.KNXLinkClosedException;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.mgmt.Destination;
import tuwien.auto.calimero.mgmt.ManagementClientImpl;
import tuwien.auto.calimero.mgmt.TransportLayer;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Objects;

/**
 * Extends the calimero-core class {@link ManagementClientImpl}
 * to send firmware update using APCI_MEMORY_WRITE_PDU and APCI_MEMORY_RESPONSE_PDU
 * with our custom UPD/UDP protocol
 * <p>
 * field {@link ManagementClientImpl#tl} and method {@link ManagementClientImpl#sendWait}
 * are package-private of calimero-core, so we can't directly access them.
 * see also
 * https://github.com/calimero-project/calimero-core/issues/105
 * <p>
 * As a workaround we get access to them by using java reflections.
 */
public class SBManagementClientImpl extends ManagementClientImpl {
    private static final int MEMORY_RESPONSE = 0x0240;
    private static final int MEMORY_WRITE = 0x0280;

    private final Logger logger;
    private final TransportLayer refTl;
    private final Method refSendWait;
    private final Method refSend;

    public SBManagementClientImpl(KNXNetworkLink link)
            throws KNXLinkClosedException, NoSuchFieldException, IllegalAccessException, NoSuchMethodException {
        super(link);
        logger = LoggerFactory.getLogger(SBManagementClientImpl.class.getName() + " " + link.getName());

        Field field = ManagementClientImpl.class.getDeclaredField("tl");
        field.setAccessible(true);
        refTl = (TransportLayer) field.get(this);
        field.setAccessible(false);

        refSendWait = ManagementClientImpl.class.getDeclaredMethod("sendWait",
                Destination.class, Priority.class, byte[].class,  int.class, int.class, int.class);
        refSendWait.setAccessible(true);

        refSend = ManagementClientImpl.class.getDeclaredMethod("send",
                Destination.class, Priority.class, byte[].class, int.class, byte[].class);
        refSend.setAccessible(true);
    }

    // for Selfbus updater
    public byte[] sendUpdateData(final Destination dst, final int cmd, final byte[] data)
            throws Throwable {

        final byte[] asdu = new byte[data.length + 3];
        asdu[0] = (byte) data.length;
        asdu[1] = 0;
        asdu[2] = (byte) cmd;
        for (int i = 0; i < data.length; ++i)
            asdu[3 + i] = data[i];

        byte[] apdu;
        try {
            if (dst.isConnectionOriented()) {
                // tl.connect(dst);
                refTl.connect(dst);
            }
            else {
                logger.error("send update data in connectionless mode, " + dst);
            }
            final byte[] send = DataUnitBuilder.createLengthOptimizedAPDU(MEMORY_WRITE, asdu);

            // final byte[] apdu = sendWait(dst, getPriority(), send, MEMORY_RESPONSE, 0, 65);
            apdu = (byte[]) refSendWait.invoke(this, dst, getPriority(), send, MEMORY_RESPONSE, 0, 65);
        } catch (IllegalAccessException | InvocationTargetException | KNXException e) {
            throw findCause(e);
        }
        return apdu;
    }
    
    private static Throwable findCause(Throwable throwable) {
        Objects.requireNonNull(throwable);
        Throwable rootCause = throwable;
        while (rootCause.getCause() != null && rootCause.getCause() != rootCause) {
            rootCause = rootCause.getCause();
        }
        return rootCause;
    }
}
