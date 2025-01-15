package org.selfbus.updater;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.IndividualAddress;
import tuwien.auto.calimero.KNXException;
import tuwien.auto.calimero.knxnetip.KNXnetIPRouting;
import tuwien.auto.calimero.knxnetip.SecureConnection;
import tuwien.auto.calimero.knxnetip.TcpConnection;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.link.KNXNetworkLinkFT12;
import tuwien.auto.calimero.link.KNXNetworkLinkIP;
import tuwien.auto.calimero.link.KNXNetworkLinkTpuart;
import tuwien.auto.calimero.link.KNXNetworkLinkUsb;
import tuwien.auto.calimero.link.medium.KNXMediumSettings;
import tuwien.auto.calimero.link.medium.TPSettings;

import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import java.util.Collections;
import java.util.Objects;

import static org.fusesource.jansi.Ansi.*;
import static org.fusesource.jansi.Ansi.Color.*;

public class SBKNXLink {
    private static final Logger logger = LoggerFactory.getLogger(SBKNXLink.class);
    private CliOptions cliOptions;

    @SuppressWarnings("unused")
    private SBKNXLink() {

    }

    public SBKNXLink(CliOptions cliOptions) {
        this.cliOptions = cliOptions;
    }

    private KNXNetworkLink createSecureTunnelingLink(InetSocketAddress local, InetSocketAddress remote,
                                             KNXMediumSettings medium) throws KNXException, InterruptedException {
        // KNX IP Secure TCP tunneling v2 connection
        logger.info("Connect using KNX IP Secure tunneling");
        byte[] deviceAuthCode = SecureConnection.hashDeviceAuthenticationPassword(cliOptions.getKnxSecureDevicePassword().toCharArray());
        byte[] userKey = SecureConnection.hashUserPassword(cliOptions.getKnxSecureUserPassword().toCharArray());
        final TcpConnection.SecureSession session;
        try (TcpConnection tcpConnection = Utils.tcpConnection(local, remote)) {
            session = tcpConnection.newSecureSession(cliOptions.getKnxSecureUserId(), userKey, deviceAuthCode);
        }
        return KNXNetworkLinkIP.newSecureTunnelingLink(session, medium);
    }

    private KNXNetworkLink createTunnelingLinkV2(InetSocketAddress local, InetSocketAddress remote,
                                         KNXMediumSettings medium) throws KNXException, InterruptedException {
        logger.info("Connect using TCP tunneling v2");
        final var session = Utils.tcpConnection(local, remote);
        return KNXNetworkLinkIP.newTunnelingLink(session, medium);
    }

    private KNXNetworkLink createTunnelingLinkV1(InetSocketAddress local, InetSocketAddress remote, boolean useNat,
                                         KNXMediumSettings medium) throws KNXException, InterruptedException {
        logger.info("{}Connect using UDP tunneling v1 (nat:{}){}", ansi().fg(YELLOW), useNat, ansi().reset());
        return KNXNetworkLinkIP.newTunnelingLink(local, remote, useNat, medium);
    }

    private KNXNetworkLink createRoutingLink(InetSocketAddress local, KNXMediumSettings medium) throws KNXException {
        logger.info("{}Connect using routing (multicast:{}){}",
                ansi().fg(YELLOW), KNXnetIPRouting.DefaultMulticast, ansi().reset());

        return KNXNetworkLinkIP.newRoutingLink(local.getAddress(), KNXnetIPRouting.DefaultMulticast, medium);
    }

    private static InetSocketAddress createLocalSocket(String host, Integer port) {
        port = Objects.requireNonNullElse(port, 0);
        if (host.isEmpty()) {
            return new InetSocketAddress(port);
        }
        else {
            return new InetSocketAddress(host, port);
        }
    }

    private static KNXMediumSettings getMedium(final String id, IndividualAddress ownAddress)
            throws UpdaterException {
        if (id.equalsIgnoreCase(KNXMediumSettings.getMediumString(KNXMediumSettings.MEDIUM_TP1))) {
            return new TPSettings(ownAddress);
        }
        else if (id.equalsIgnoreCase(KNXMediumSettings.getMediumString(KNXMediumSettings.MEDIUM_RF))) {
            throw new UpdaterException(String.format("medium '%s' not implemented", id));
            //return new RFSettings(ownAddress);
        }
        //else if (id.equalsIgnoreCase("tp0"))
        //	return TPSettings.TP0;
        //else if (id.equalsIgnoreCase("p110"))
        //	return new PLSettings(false);
        //else if (id.equalsIgnoreCase("p132"))
        //	return new PLSettings(true);
        else
            throw new UpdaterException("unknown medium");
    }

    private InetAddress resolveHost(final String host) throws UnknownHostException {
        InetAddress res = InetAddress.getByName(host);
        logger.debug("Resolved {} with {}", host, res);
        return res;
    }

    /**
     * Creates the KNX network link to access the network specified in
     * <code>options</code>.
     * <p>
     *
     * @return the KNX network link
     * @throws KNXException         on problems on link creation
     * @throws InterruptedException on interrupted thread
     */
    public KNXNetworkLink openLink() throws KNXException, InterruptedException,
            UnknownHostException, UpdaterException {
        return doOpenLink();
    }

    private KNXNetworkLink doOpenLink() throws KNXException,
            InterruptedException, UnknownHostException, UpdaterException {
        final KNXMediumSettings medium = getMedium(cliOptions.getMedium(), cliOptions.getOwnPhysicalAddress());
        logger.debug("Creating KNX network link {}", medium);
        if (!cliOptions.getFt12SerialPort().isEmpty()) {
            // create FT1.2 network link
            return new KNXNetworkLinkFT12(cliOptions.getFt12SerialPort(), medium);
        } else if (!cliOptions.getTpuartSerialPort().isEmpty()) {
            // create TPUART network link
            KNXNetworkLinkTpuart linkTpuart = new KNXNetworkLinkTpuart(cliOptions.getTpuartSerialPort(), medium, Collections.emptyList());
            linkTpuart.addAddress(cliOptions.getOwnPhysicalAddress()); //\todo check if this is rly needed
            return linkTpuart;
        } else if (!cliOptions.getUsbVendorIdAndProductId().isEmpty()) {
            // create USB network link
            return new KNXNetworkLinkUsb(cliOptions.getUsbVendorIdAndProductId(), medium);
        }

        // create local and remote socket address for network link
        InetSocketAddress local = createLocalSocket(cliOptions.getLocalhost(), cliOptions.getLocalPort());

        final InetSocketAddress remote = new InetSocketAddress(resolveHost(cliOptions.getKnxInterface()), cliOptions.getPort());

        // Connect using KNX IP Secure
        if ((!cliOptions.getKnxSecureDevicePassword().isEmpty()) && (!cliOptions.getKnxSecureUserPassword().isEmpty())) {
            return createSecureTunnelingLink(local, remote, medium);
        }

        if (cliOptions.getTunnelingV2isSet()) {
            return createTunnelingLinkV2(local, remote, medium);
        }

        if (cliOptions.getTunnelingV1isSet()) {
            return createTunnelingLinkV1(local, remote, cliOptions.getNatIsSet(), medium);
        }

        if (cliOptions.getRoutingIsSet()) {
            return createRoutingLink(local, medium);
        }

        // try unsecure TCP tunneling v2 connection
        try {
            KNXNetworkLink testLink = createTunnelingLinkV2(local, remote, medium);
            cliOptions.setTunnelingV2isSet(true);
            return testLink;
        } catch (final KNXException e) {
            logger.info("failed with {}", e.toString());
            cliOptions.setTunnelingV2isSet(false);
        }

        // try unsecure UDP tunneling v1 connection with nat option set on cli
        try {
            KNXNetworkLink testLink = createTunnelingLinkV1(local, remote, cliOptions.getNatIsSet(), medium);
            cliOptions.setTunnelingV1isSet(true);
            return testLink;
        } catch (final KNXException e) {
            cliOptions.setTunnelingV1isSet(false);
            logger.info("{}failed with {}{}", ansi().fg(YELLOW), e, ansi().reset());
        }

        // last chance try unsecure UDP tunneling v1 connection with INVERTED nat option set on cli
        KNXNetworkLink testLink = createTunnelingLinkV1(local, remote, !cliOptions.getNatIsSet(), medium);
        cliOptions.setNatIsSet(!cliOptions.getNatIsSet());
        return testLink;
    }
}