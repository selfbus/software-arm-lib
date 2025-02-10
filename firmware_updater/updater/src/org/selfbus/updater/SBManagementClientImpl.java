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

import java.time.Duration;

import static org.selfbus.updater.Mcu.MAX_ASDU_LENGTH;
import static tuwien.auto.calimero.mgmt.Destination.State.OpenIdle;

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

    private class SBLinkListener implements NetworkLinkListener
    {
        private final Logger linkLogger;

        SBLinkListener() {
            this.linkLogger = (Logger) LoggerFactory.getLogger(SBLinkListener.class.getName() + " " + link.getName());
        }

        public void indication(final FrameEvent e)
        {
            CEMILData cemilData = getCEMILData(e, linkLogger);
            if (cemilData == null) {
                return;
            }

            if (cemilData.getPayload().length < 1) {
                linkLogger.debug("no payload");
                return;
            }

            final int tpdu = cemilData.getPayload()[0] & 0xff;
            if (tpdu == 0x80) {
                linkLogger.debug("indication {} -> {} T_Connect", cemilData.getSource(), cemilData.getDestination());
                return;
            }
            else if (tpdu == 0x81) {
                linkLogger.debug("indication {} -> {} T_Disconnect", cemilData.getSource(), cemilData.getDestination());
                return;
            }

            final int ctrl = tpdu & 0xc0; // is it connection oriented? (1100 0000)
            if (ctrl != 0) {
                final int conControl = tpdu & 0xc3; // clear sequence number bits (1100 0011)
                final int sequenceNumber = (tpdu & 0x3c) >> 2; // get sequence number and shift right by 2 (0011 1100)
                switch (conControl) {
                    case 0xC2:
                        linkLogger.debug("indication {} -> {} T_Ack #{}", cemilData.getSource(), cemilData.getDestination(), sequenceNumber);
                        break;

                    case 0xC3:
                        linkLogger.debug("indication {} -> {} T_Nack #{}", cemilData.getSource(), cemilData.getDestination(), sequenceNumber);
                        break;

                    case 0x40:
                    case 0x41:
                    case 0x42:
                    case 0x43:
                        linkLogger.debug("indication {} -> {} T_Data_Connected #{}", cemilData.getSource(), cemilData.getDestination(), sequenceNumber);
                        break;

                    default:
                        // This should never happen
                        linkLogger.warn("indication {} -> {} conControl == {} tpdu == {} #{}", cemilData.getSource(), cemilData.getDestination(),
                                String.format("0x%X", conControl), String.format("0x%X", tpdu), sequenceNumber);
                        break;
                }
            }
            else {
                final KNXAddress dst = cemilData.getDestination();
                // check for broadcast or group
                if ((dst instanceof GroupAddress) && (dst.getRawAddress() == 0)) {
                    linkLogger.debug("broadcast {} -> {}", cemilData.getSource(), dst);
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
        public void broadcast(final FrameEvent e) {}

        @Override
        public void dataConnected(final FrameEvent e) {}

        @Override
        public void dataIndividual(final FrameEvent e) {}

        @Override
        public void disconnected(final Destination d) {
            transportLogger.debug("disconnected {}", d);
        }

        @Override
        public void group(final FrameEvent e) {}

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
    private final Logger logger;

    KNXNetworkLink link;

    public SBManagementClientImpl(KNXNetworkLink link)
            throws KNXLinkClosedException {
        super(link);
        logger = (Logger) LoggerFactory.getLogger(SBManagementClientImpl.class.getName() + " " + link.getName());
        this.link = link;
        SBLinkListener linkListener = new SBLinkListener();
        this.link.addLinkListener(linkListener);
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

    @Override
    protected byte[] sendWait(final Destination d, final Priority p, final byte[] apdu, final int responseServiceType,
                              final int minAsduLen, final int maxAsduLen, final Duration timeout) throws KNXDisconnectException,
            KNXTimeoutException, KNXInvalidResponseException, KNXLinkClosedException, InterruptedException {
        byte[] received;
        try {
            received = super.sendWait(d, p, apdu, responseServiceType, minAsduLen, maxAsduLen, timeout);
        }
        catch (KNXTimeoutException e) {
            if (d.getState() != OpenIdle)
            {
                logger.debug("Critical destination state: {}", d);
            }
            throw e;
        }
        return received;
    }
}
