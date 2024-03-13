package org.selfbus.updater;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.IndividualAddress;
import tuwien.auto.calimero.KNXException;
import tuwien.auto.calimero.KNXIllegalArgumentException;
import tuwien.auto.calimero.knxnetip.KNXnetIPRouting;
import tuwien.auto.calimero.knxnetip.SecureConnection;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.link.KNXNetworkLinkFT12;
import tuwien.auto.calimero.link.KNXNetworkLinkIP;
import tuwien.auto.calimero.link.KNXNetworkLinkTpuart;
import tuwien.auto.calimero.link.medium.KNXMediumSettings;
import tuwien.auto.calimero.link.medium.RFSettings;
import tuwien.auto.calimero.link.medium.TPSettings;

import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.util.Collections;

public class SBKNXLink {
    private static final Logger logger = LoggerFactory.getLogger(CliOptions.class.getName());
    private CliOptions cliOptions;

    public SBKNXLink() {

    }

    private KNXNetworkLink createSecureTunnelingLink(InetSocketAddress local, InetSocketAddress remote,
                                             KNXMediumSettings medium) throws KNXException, InterruptedException {
        // KNX IP Secure TCP tunneling v2 connection
        logger.info("Connect using KNX IP Secure tunneling...");
        byte[] deviceAuthCode = SecureConnection.hashDeviceAuthenticationPassword(cliOptions.devicePassword().toCharArray());
        byte[] userKey = SecureConnection.hashUserPassword(cliOptions.userPassword().toCharArray());
        final var session = Utils.tcpConnection(local, remote).newSecureSession(cliOptions.userId(), userKey, deviceAuthCode);
        return KNXNetworkLinkIP.newSecureTunnelingLink(session, medium);
    }

    private KNXNetworkLink createTunnelingLinkV2(InetSocketAddress local, InetSocketAddress remote,
                                         KNXMediumSettings medium) throws KNXException, InterruptedException {
        logger.info("Connect using TCP tunneling v2...");
        final var session = Utils.tcpConnection(local, remote);
        return KNXNetworkLinkIP.newTunnelingLink(session, medium);
    }

    private KNXNetworkLink createTunnelingLinkV1(InetSocketAddress local, InetSocketAddress remote, boolean useNat,
                                         KNXMediumSettings medium) throws KNXException, InterruptedException {
        logger.info("{}Connect using UDP tunneling v1 (nat:{})...{}", ConColors.YELLOW, useNat, ConColors.RESET);
        return KNXNetworkLinkIP.newTunnelingLink(local, remote, useNat, medium);
    }

    private KNXNetworkLink createRoutingLink(InetSocketAddress local, KNXMediumSettings medium) throws KNXException {
        logger.info("{}Connect using routing (multicast:{})...{}", ConColors.YELLOW, KNXnetIPRouting.DefaultMulticast, ConColors.RESET);

        return KNXNetworkLinkIP.newRoutingLink(local.getAddress(), KNXnetIPRouting.DefaultMulticast, medium);
    }

    private static InetSocketAddress createLocalSocket(final InetAddress host, final Integer port) {
        final int p = port != null ? port.intValue() : 0;
        return host != null ? new InetSocketAddress(host, p) : new InetSocketAddress(p);
    }

    private static KNXMediumSettings getMedium(final String id, IndividualAddress ownAddress) {
        if (id.equals("tp1")) {
            return new TPSettings(ownAddress);
        } else if (id.equals("rf")) {
            return new RFSettings(null);
        }
        //else if (id.equals("tp0"))
        //	return TPSettings.TP0;
        //else if (id.equals("p110"))
        //	return new PLSettings(false);
        //else if (id.equals("p132"))
        //	return new PLSettings(true);
        else
            throw new KNXIllegalArgumentException("unknown medium");
    }

    public void setCliOptions(CliOptions cliOptions) {
        this.cliOptions = cliOptions;
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
    public KNXNetworkLink openLink() throws KNXException,
            InterruptedException, UpdaterException {
        final KNXMediumSettings medium = getMedium(cliOptions.medium(), cliOptions.ownAddress());
        logger.debug("Creating KNX network link {}...", medium);
        if (!cliOptions.ft12().isEmpty()) {
            // create FT1.2 network link
            try {
                return new KNXNetworkLinkFT12(Integer.parseInt(cliOptions.ft12()), medium);
            } catch (final NumberFormatException e) {
                return new KNXNetworkLinkFT12(cliOptions.ft12(), medium);
            }
        } else if (!cliOptions.tpuart().isEmpty()) {
            // create TPUART network link
            KNXNetworkLinkTpuart linkTpuart = new KNXNetworkLinkTpuart(cliOptions.tpuart(), medium, Collections.emptyList());
            linkTpuart.addAddress(cliOptions.ownAddress()); //\todo check if this is rly needed
            return linkTpuart;
        }

        // create local and remote socket address for network link
        InetSocketAddress local = createLocalSocket(cliOptions.localhost(), cliOptions.localport());

        final InetSocketAddress remote = new InetSocketAddress(cliOptions.knxInterface(), cliOptions.port());

        // Connect using KNX IP Secure
        if ((!cliOptions.devicePassword().isEmpty()) && (!cliOptions.userPassword().isEmpty())) {
            return createSecureTunnelingLink(local, remote, medium);
        }

        if (cliOptions.tunnelingV2()) {
            return createTunnelingLinkV2(local, remote, medium);
        }

        if (cliOptions.tunnelingV1()) {
            return createTunnelingLinkV1(local, remote, cliOptions.nat(), medium);
        }

        if (cliOptions.routing()) {
            return createRoutingLink(local, medium);
        }

        // try unsecure TCP tunneling v2 connection
        try {
            return createTunnelingLinkV2(local, remote, medium);
        } catch (final KNXException | InterruptedException e) {
            logger.info("failed with {}", e.toString());
        }

        // try unsecure UDP tunneling v1 connection with nat option set on cli
        try {
            return createTunnelingLinkV1(local, remote, cliOptions.nat(), medium);
        } catch (final KNXException | InterruptedException e) {
            logger.info("{}failed with {}{}", ConColors.YELLOW, e, ConColors.RESET);
        }

        // last chance try unsecure UDP tunneling v1 connection with INVERTED nat option set on cli
        return createTunnelingLinkV1(local, remote, !cliOptions.nat(), medium);
    }
}