package org.selfbus.updater;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import io.calimero.DataUnitBuilder;
import io.calimero.IndividualAddress;
import io.calimero.KNXInvalidResponseException;
import io.calimero.KNXTimeoutException;
import io.calimero.link.KNXLinkClosedException;
import io.calimero.link.KNXNetworkLink;
import io.calimero.mgmt.Destination;
import io.calimero.mgmt.KNXDisconnectException;
import io.calimero.mgmt.ManagementClientImpl;

import java.util.List;
import java.util.Optional;
import java.util.function.BiFunction;

import static org.selfbus.updater.Mcu.MAX_ASDU_LENGTH;
import static org.selfbus.updater.Mcu.MAX_PAYLOAD;

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
    private final Logger logger;

    public SBManagementClientImpl(KNXNetworkLink link)
            throws KNXLinkClosedException {
        super(link);
        logger = LoggerFactory.getLogger(SBManagementClientImpl.class.getName() + " " + link.getName());
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

        BiFunction<IndividualAddress, byte[], Optional<byte[]>> responseFilter = (responder, apdu) -> {
            if (responder.equals(dst.getAddress())) {
                return Optional.of(apdu);
            }
            else {
                return Optional.empty();
            }
        };
        final byte[] asdu = prepareAsdu(cmd, data);

        byte[] send;
        if (asdu.length > MAX_ASDU_LENGTH) {
            // prevent creation of long frames, sblib doesn't support them yet
            throw new UpdaterException(String.format("asdu.length (%d) exceeds MAX_ASDU_LENGTH(%d)",
                    asdu.length, MAX_ASDU_LENGTH));
        }
        send = DataUnitBuilder.createAPDU(apciWrite, asdu);

        final byte[] response = this.sendWait(dst, getPriority(), send, apciResponse, 2, MAX_ASDU_LENGTH, responseTimeout());
        return response;
    }
}
