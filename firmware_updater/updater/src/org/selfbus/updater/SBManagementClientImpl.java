package org.selfbus.updater;

import ch.qos.logback.classic.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.*;
import tuwien.auto.calimero.cemi.CEMI;
import tuwien.auto.calimero.cemi.CEMILData;
import tuwien.auto.calimero.link.*;
import tuwien.auto.calimero.mgmt.Destination;
import tuwien.auto.calimero.mgmt.KNXDisconnectException;
import tuwien.auto.calimero.mgmt.ManagementClientImpl;
import tuwien.auto.calimero.mgmt.TransportListener;

import static org.selfbus.updater.Mcu.MAX_ASDU_LENGTH;

/**
 * Extends the calimero-core class {@link ManagementClientImpl}
 * to send firmware update using 
 * APCI_USERMSG_MANUFACTURER_0 and APCI_USERMSG_MANUFACTURER_6
 * with our custom UPD/UDP protocol.
 */
public class SBManagementClientImpl extends ManagementClientImpl {

    private CEMILData getCEMILData(final FrameEvent e, Logger eventLogger) {
        final CEMI cemi = e.getFrame();
        if (!(cemi instanceof final CEMILData linkData)) {
            eventLogger.debug("not a CEMILData");
            return null;
        }
        return linkData;
    }

    /**
     * Logs details about a frame event.
     * <p>
     * This method checks if the frame retrieved from the {@link FrameEvent} is of type {@link CEMILData}.
     * If the frame is valid, it logs the provided message along with the frame data.
     * </p>
     *
     * @param message     A descriptive message to include in the log.
     * @param e           The {@link FrameEvent} containing the frame to be logged.
     * @param eventLogger The logger to use for logging the frame details.
     * @return The {@link CEMILData} instance if the frame is valid; {@code null} otherwise.
     */
    private CEMILData logEvent(String message, final FrameEvent e, final Logger eventLogger) {
        CEMILData linkData = getCEMILData(e, eventLogger);
        if (linkData == null) {
            return null;
        }

        eventLogger.debug("{} : {}", message, linkData);
        if (linkData.getSource().equals(link.getKNXMedium().getDeviceAddress())) {
            eventLogger.debug("source == device address!");
        }
        return linkData;
    }

    private class SBLinkListener implements NetworkLinkListener
    {
        private final Logger linkLogger;

        SBLinkListener() {
            this.linkLogger = (Logger) LoggerFactory.getLogger(SBLinkListener.class.getName() + " " + link.getName());
        }

        public void indication(final FrameEvent e)
        {
            CEMILData cemilData = logEvent("indication", e, linkLogger);
            if (cemilData == null) {
                return;
            }

            if (cemilData.getPayload().length < 1) {
                linkLogger.debug("no payload");
                return;
            }

            int ctrl = cemilData.getPayload()[0] & 0xfc;
            if (ctrl == 0) {
                final KNXAddress dst = cemilData.getDestination();
                if (dst instanceof GroupAddress)
                    // check for broadcast or group
                    if (dst.getRawAddress() == 0) {
                        linkLogger.debug("broadcast");
                    }
                    else {
                        linkLogger.debug("group");
                    }
                else
                    // individual
                    linkLogger.debug("individual");
            }
            else {
                int conControl = cemilData.getPayload()[0] & 0xc3;
                int sequenceNumber = (cemilData.getPayload()[0] & 0x3c) >> 2;
                switch (conControl) {
                    case 0x80:
                        linkLogger.debug("indication {} -> {} T_Connect", cemilData.getSource(), cemilData.getDestination());
                        break;

                    case 0x81:
                        linkLogger.debug("indication {} -> {} T_Disconnect", cemilData.getSource(), cemilData.getDestination());
                        break;

                    case 0xC2:
                        linkLogger.debug("indication {} -> {} T_Ack #{}", cemilData.getSource(), cemilData.getDestination(), sequenceNumber);
                        break;

                    case 0xC3:
                        linkLogger.debug("indication {} -> {} T_Nack #{}", cemilData.getSource(), cemilData.getDestination(), sequenceNumber);
                        break;

                    case 0x42:
                        linkLogger.debug("indication {} -> {} T_Data_Connected #{}", cemilData.getSource(), cemilData.getDestination(), sequenceNumber);
                        break;

                    default:
                        linkLogger.debug("indication {} -> {} conControl == 0x{} #{}", cemilData.getSource(), cemilData.getDestination(),
                                String.format("%X", conControl), sequenceNumber);
                        break;
                }
            }
        }

        @Override
        public void linkClosed(final CloseEvent e)
        {
            linkLogger.debug("attached link was closed");
        }
    }

    private class SBTransportListener implements TransportListener
    {
        private final Logger transportLogger;

        SBTransportListener() {
            this.transportLogger = (Logger) LoggerFactory.getLogger(SBTransportListener.class.getName() + " " + link.getName());
        }

        @Override
        public void broadcast(final FrameEvent e)
        {
            logEvent("broadcast", e, transportLogger);
        }

        @Override
        public void dataConnected(final FrameEvent e)
        {
            logEvent("dataConnected", e, transportLogger);
        }

        @Override
        public void dataIndividual(final FrameEvent e)
        {
            logEvent("dataIndividual", e, transportLogger);
        }

        @Override
        public void disconnected(final Destination d) {
            transportLogger.debug("disconnected {}", d);
        }

        @Override
        public void group(final FrameEvent e) {
            transportLogger.debug("group {}", e);
        }

        @Override
        public void detached(final DetachEvent e) {
            transportLogger.debug("detached {}", e);
        }

        @Override
        public void linkClosed(final CloseEvent e)
        {
            transportLogger.debug("linkClosed {}", e);
        }
    }

    private static final int USERMSG_MANUFACTURER_0_WRITE    = 0x2F8;
    private static final int USERMSG_MANUFACTURER_6_RESPONSE = 0x2FE;
    private static final int apciWrite = USERMSG_MANUFACTURER_0_WRITE;
    private static final int apciResponse = USERMSG_MANUFACTURER_6_RESPONSE;
    //private final Logger logger; // uncomment when needed

    KNXNetworkLink link;

    public SBManagementClientImpl(KNXNetworkLink link)
            throws KNXLinkClosedException {
        super(link);
        this.link = link;
        SBLinkListener linkListener = new SBLinkListener();
        this.link.addLinkListener(linkListener);
        //logger = (Logger) LoggerFactory.getLogger(SBManagementClientImpl.class.getName() + " " + link.getName());
        SBTransportListener transportListener = new SBTransportListener();
        this.transportLayer().addTransportListener(transportListener);
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
            throws KNXTimeoutException, KNXLinkClosedException, KNXInvalidResponseException, KNXDisconnectException,
            InterruptedException, UpdaterException {
        final byte[] asdu = prepareAsdu(cmd, data);

        byte[] send;
        if (asdu.length > MAX_ASDU_LENGTH) {
            // prevent creation of long frames, sblib doesn't support them yet
            throw new UpdaterException(String.format("asdu.length (%d) exceeds MAX_ASDU_LENGTH (%d)",
                    asdu.length, MAX_ASDU_LENGTH));
        }
        send = DataUnitBuilder.createAPDU(apciWrite, asdu);

        return this.sendWait(dst, getPriority(), send, apciResponse, 2, MAX_ASDU_LENGTH, responseTimeout());
    }
}
