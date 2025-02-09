package org.selfbus.updater;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.KNXException;
import tuwien.auto.calimero.knxnetip.Discoverer;
import tuwien.auto.calimero.knxnetip.servicetype.SearchResponse;
import tuwien.auto.calimero.link.medium.KNXMediumSettings;
import tuwien.auto.calimero.serial.usb.Device;
import tuwien.auto.calimero.serial.usb.UsbConnection;
import tuwien.auto.calimero.serial.usb.UsbConnectionFactory;

import java.time.Duration;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.concurrent.ExecutionException;

public class DiscoverKnxInterfaces {
    private final static Logger logger = LoggerFactory.getLogger(DiscoverKnxInterfaces.class);

    public static List<Discoverer.Result<SearchResponse>> getAllnetIPInterfaces() throws InterruptedException {

        List<Discoverer.Result<SearchResponse>> interfacesResult = null;
        try {
            // set true to be aware of Network Address Translation (NAT) during discovery
            final boolean useNAT = false;
            interfacesResult = Discoverer.udp(useNAT).timeout(Duration.ofSeconds(3)).search().get();
        }
        catch (ExecutionException e) {
            logger.warn("Error during KNXnet/IP discovery: {}", e.getMessage());
        }

        return interfacesResult;
    }

    public static Set<Device> getUsbInterfaces() throws InterruptedException {
        Set<Device> knxUsbDevices = UsbConnectionFactory.attachedKnxUsbDevices();
        Iterator<Device> iterator = knxUsbDevices.iterator();
        while (iterator.hasNext()) {
            Device d = iterator.next();
            try {
                // check knx medium, defaults to TP1
                try (UsbConnection c = UsbConnectionFactory.open(d)) {
                    if (c.deviceDescriptor().medium().getMedium() != KNXMediumSettings.MEDIUM_TP1) {
                        iterator.remove();
                    }
                }
                catch (KNXException e) {
                    iterator.remove();
                    logger.warn("Reading KNX device descriptor of {} failed. ({}: {})",
                            d,  e.getClass().getSimpleName(), e.getMessage());
                    logger.debug("", e);
                }

            }
            catch (final RuntimeException e) {
                iterator.remove();
                logger.error("error: {}", e.getMessage());
            }
        }
        return knxUsbDevices;
    }

    public static void toText(List<Discoverer.Result<SearchResponse>> ifaceList) {
        for (Discoverer.Result<SearchResponse> r : ifaceList) {
            SearchResponse sr = r.response();
            logger.info("Found IP interface: {}", sr.getDevice().getName());
            logger.info("\t{}", sr.toString().replace(", ", "\n\t"));
        }
    }

    public static void toText(Set<Device> usbInterfaces) {
        for (final var d : usbInterfaces) {
            final String vp = String.format("%04x:%04x", d.vendorId(), d.productId());
            logger.info("Found USB Interface: {} {} S/N {} VID/PID {}",
                    d.manufacturer(), d.product(), d.serialNumber(), vp);
        }
    }
}
