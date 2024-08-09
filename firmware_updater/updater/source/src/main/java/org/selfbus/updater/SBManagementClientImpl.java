package org.selfbus.updater;

import tuwien.auto.calimero.DataUnitBuilder;
import tuwien.auto.calimero.KNXInvalidResponseException;
import tuwien.auto.calimero.KNXTimeoutException;
import tuwien.auto.calimero.link.KNXLinkClosedException;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.mgmt.Destination;
import tuwien.auto.calimero.mgmt.KNXDisconnectException;
import tuwien.auto.calimero.mgmt.ManagementClientImpl;

import static org.selfbus.updater.Mcu.MAX_ASDU_LENGTH;

/**
 * Extends the calimero-core class {@link ManagementClientImpl}
 * to send firmware update using 
 * APCI_USERMSG_MANUFACTURER_0 and APCI_USERMSG_MANUFACTURER_6
 * with our custom UPD/UDP protocol.
 */
public class SBManagementClientImpl extends ManagementClientImpl {
    private static final int USERMSG_MANUFACTURER_0_WRITE    = 0x2F8;
    private static final int USERMSG_MANUFACTURER_6_RESPONSE = 0x2FE;
    private static final int apciWrite = USERMSG_MANUFACTURER_0_WRITE;
    private static final int apciResponse = USERMSG_MANUFACTURER_6_RESPONSE;
    //private final Logger logger; // uncomment when needed

    public SBManagementClientImpl(KNXNetworkLink link)
            throws KNXLinkClosedException {
        super(link);
        //logger = LoggerFactory.getLogger(SBManagementClientImpl.class.getName() + " " + link.getName());
    }

    private byte[] prepareAsdu(final int cmd, final byte[] data) {
        byte[] asdu;
        asdu = new byte[data.length + 1];
        asdu[0] = (byte) cmd;
        System.arraycopy(data, 0, asdu, 1, data.length);
        return asdu;
    }

    // for Selfbus updater
    public byte[] sendUpdateData(final Destination dst, final int cmd, final byte[] data)
            throws KNXTimeoutException, KNXLinkClosedException, KNXInvalidResponseException, KNXDisconnectException, InterruptedException, UpdaterException {
        final byte[] asdu = prepareAsdu(cmd, data);

        byte[] send;
        if (asdu.length > MAX_ASDU_LENGTH) {
            // prevent creation of long frames, sblib doesn't support them yet
            throw new UpdaterException(String.format("asdu.length (%d) exceeds MAX_ASDU_LENGTH(%d)",
                    asdu.length, MAX_ASDU_LENGTH));
        }
        send = DataUnitBuilder.createAPDU(apciWrite, asdu);

        return this.sendWait(dst, getPriority(), send, apciResponse, 2, MAX_ASDU_LENGTH, responseTimeout());
    }
}
