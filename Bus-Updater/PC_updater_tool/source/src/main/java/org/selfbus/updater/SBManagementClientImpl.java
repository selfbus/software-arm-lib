package org.selfbus.updater;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.DataUnitBuilder;
import tuwien.auto.calimero.IndividualAddress;
import tuwien.auto.calimero.KNXInvalidResponseException;
import tuwien.auto.calimero.KNXTimeoutException;
import tuwien.auto.calimero.link.KNXLinkClosedException;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.mgmt.Destination;
import tuwien.auto.calimero.mgmt.KNXDisconnectException;
import tuwien.auto.calimero.mgmt.ManagementClientImpl;

import java.util.List;
import java.util.Optional;
import java.util.function.BiFunction;

/**
 * Extends the calimero-core class {@link ManagementClientImpl}
 * to send firmware update using APCI_MEMORY_WRITE_PDU and APCI_MEMORY_RESPONSE_PDU
 * with our custom UPD/UDP protocol.
 */
public class SBManagementClientImpl extends ManagementClientImpl {
    private static final int MEMORY_RESPONSE = 0x0240;
    private static final int MEMORY_WRITE = 0x0280;

    private final Logger logger;

    public SBManagementClientImpl(KNXNetworkLink link)
            throws KNXLinkClosedException {
        super(link);
        logger = LoggerFactory.getLogger(SBManagementClientImpl.class.getName() + " " + link.getName());
    }

    // for Selfbus updater
    public byte[] sendUpdateData(final Destination dst, final int cmd, final byte[] data)
            throws KNXTimeoutException, KNXLinkClosedException, KNXInvalidResponseException, KNXDisconnectException, InterruptedException {

        BiFunction<IndividualAddress, byte[], Optional<byte[]>> responseFilter = (responder, apdu) -> {
            if (responder.equals(dst.getAddress())) {
                return Optional.of(apdu);
            }
            else {
                return Optional.empty();
            }
        };

        final byte[] asdu = new byte[data.length + 3];
        asdu[0] = (byte) data.length;
        asdu[1] = 0;
        asdu[2] = (byte) cmd;
        System.arraycopy(data, 0, asdu, 3, data.length);
        final byte[] send = DataUnitBuilder.createLengthOptimizedAPDU(MEMORY_WRITE, asdu);
        long startSend = this.send(dst, getPriority(), send, MEMORY_RESPONSE);

        List<byte[]> response = this.waitForResponses(MEMORY_RESPONSE, 0, 65, startSend, responseTimeout(), true, responseFilter);
        return response.get(0);
    }
}
