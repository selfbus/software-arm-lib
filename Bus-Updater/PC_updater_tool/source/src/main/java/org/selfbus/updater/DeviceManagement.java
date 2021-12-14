package org.selfbus.updater;

import org.selfbus.updater.bootloader.BootDescriptor;
import org.selfbus.updater.bootloader.BootLoaderIdentity;
import org.selfbus.updater.upd.UPDCommand;
import org.selfbus.updater.upd.UPDProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.IndividualAddress;
import tuwien.auto.calimero.KNXException;
import tuwien.auto.calimero.KNXRemoteException;
import tuwien.auto.calimero.KNXTimeoutException;
import tuwien.auto.calimero.link.KNXLinkClosedException;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.mgmt.Destination;
import tuwien.auto.calimero.mgmt.KNXDisconnectException;
import tuwien.auto.calimero.mgmt.UpdatableManagementClientImpl;

import java.util.Arrays;

/**
 * Provides methods to send firmware update telegrams to the bootloader (MCU)
 */
public final class DeviceManagement {
    private static final int RESTART_ERASE_CODE = 7; //!< EraseCode for the APCI_MASTER_RESET_PDU (valid from 1..7)
    private static final int RESTART_CHANNEL = 255;  //!< Channelnumber for the APCI_MASTER_RESET_PDU
    public static final int MAX_UPD_COMMAND_RETRY = 3; //!< default maximum retries a UPD command is sent to the client

    DeviceManagement (){}
    private final static Logger logger = LoggerFactory.getLogger(DeviceManagement.class.getName());

    public static void restartProgrammingDevice(UpdatableManagementClientImpl m, Destination dest)
            throws KNXTimeoutException, KNXLinkClosedException, InterruptedException {
        logger.warn("restarting device {}", dest);
        m.restart(dest);
    }

    /**
     * Restarts the <code>device</code> in normal or Bootloader mode
     * @param link
     *          the KNX network link  to communicate with the device
     * @param device
     *          the IndividualAddress of the device to restart
     * @return true if successful, otherwise false
     */
    public static boolean restartDeviceToBootloader(KNXNetworkLink link, IndividualAddress device) throws KNXLinkClosedException {
        UpdatableManagementClientImpl mcDevice = new UpdatableManagementClientImpl(link);
        Destination dest;
        dest = mcDevice.createDestination(device, true);
        try {
            logger.info("\nRestarting device {} to bootloader mode using {}", device, link);
            int restartProcessTime =  mcDevice.restart(dest, RESTART_ERASE_CODE, RESTART_CHANNEL);
            mcDevice.detach();
            logger.info("Device {} reported {}{} seconds{} for restarting", device, ConColors.BRIGHT_GREEN, restartProcessTime, ConColors.RESET);
            while (restartProcessTime > 0) {
                Thread.sleep(1000);
                System.out.printf("%s%s%s", ConColors.BRIGHT_GREEN, Utils.PROGRESS_MARKER, ConColors.RESET);
                restartProcessTime--;

            }
            System.out.println();
            logger.info("Device {} should now have started into bootloader mode.", device);
            return true;
        } catch (final KNXException e) {
            logger.info("{}Restart status of device {} unknown. {}{}", ConColors.BRIGHT_RED, device, e.getMessage(), ConColors.RESET);
            logger.debug("KNXException ", e);
        } catch (final InterruptedException e) {
            logger.error("InterruptedException ", e);
        } finally {
            mcDevice.detach();
        }
        return false;
    }

    public static  byte[] requestUIDFromDevice(UpdatableManagementClientImpl m, Destination dest)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nRequesting UID from {}...", dest.getAddress());
        //byte[] result = m.sendUpdateData(dest, UPDCommand.REQUEST_UID.id, new byte[0]);
        byte[] result = sendWithRetry(m, dest, UPDCommand.REQUEST_UID, new byte[] {0}, MAX_UPD_COMMAND_RETRY);
        if (result[3] != UPDCommand.RESPONSE_UID.id) {
            UPDProtocol.checkResult(result, true);
            DeviceManagement.restartProgrammingDevice(m, dest);
            throw new UpdaterException("Requesting UID failed!");
        }

        byte[] uid;
        if ((result.length >= UPDProtocol.UID_LENGTH_USED) && (result.length <= UPDProtocol.UID_LENGTH_MAX)){
            uid = Arrays.copyOfRange(result, 4, UPDProtocol.UID_LENGTH_USED + 4);
            logger.info("  got: {} length {}", Utils.byteArrayToHex(uid), uid.length);
            return uid;
        } else {
            uid = Arrays.copyOfRange(result, 4, result.length - 4);
            logger.error("Request UID failed {} result.length={}, UID_LENGTH_USED={}, UID_LENGTH_MAX={}",
                    Utils.byteArrayToHex(uid),uid.length, UPDProtocol.UID_LENGTH_USED, UPDProtocol.UID_LENGTH_MAX);
            DeviceManagement.restartProgrammingDevice(m, dest);
            throw new UpdaterException("Selfbus update failed.");
        }
    }

    public static BootLoaderIdentity requestBootLoaderIdentity(UpdatableManagementClientImpl m, Destination dest)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nRequesting Bootloader Identity...");

        // byte[] result = m.sendUpdateData(dest, UPDCommand.REQUEST_BL_IDENTITY.id, new byte[] {0});
        byte[] result = sendWithRetry(m, dest, UPDCommand.REQUEST_BL_IDENTITY, new byte[] {0}, MAX_UPD_COMMAND_RETRY);
        if (result[3] != UPDCommand.RESPONSE_BL_IDENTITY.id)
        {
            UPDProtocol.checkResult(result);
            DeviceManagement.restartProgrammingDevice(m, dest);
            throw new UpdaterException("Requesting Bootloader Identity failed!");
        }

        BootLoaderIdentity bl = BootLoaderIdentity.fromArray(Arrays.copyOfRange(result, 4, result.length));
        logger.info("  Device Bootloader: {}{}{}", ConColors.BRIGHT_YELLOW, bl, ConColors.RESET);
        if (bl.versionMajor() < ToolInfo.minMajorVersionBootloader())
        {
            logger.error("{}Bootloader version {} not compatible, please update Bootloader to version {} or higher{}",
                    ConColors.RED, bl.version(), ToolInfo.minVersionBootloader(), ConColors.RESET);
            throw new UpdaterException("Bootloader major version not compatible!");
        }
        else if (bl.versionMinor() < ToolInfo.minMinorVersionBootloader()) {
            logger.error("{}Bootloader version {} not compatible, please update Bootloader to version {} or higher{}",
                    ConColors.RED, bl.version(), ToolInfo.minVersionBootloader(), ConColors.RESET);
            throw new UpdaterException("Bootloader minor version not compatible!");
        }
        return bl;
    }

    public static BootDescriptor requestBootDescriptor(UpdatableManagementClientImpl m, Destination dest)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nRequesting Boot Descriptor...");
        //byte[] result = m.sendUpdateData(dest, UPDCommand.REQUEST_BOOT_DESC.id, new byte[] {0});
        byte[] result = sendWithRetry(m, dest, UPDCommand.REQUEST_BOOT_DESC, new byte[] {0}, MAX_UPD_COMMAND_RETRY);
        if (result[3] != UPDCommand.RESPONSE_BOOT_DESC.id) {
            UPDProtocol.checkResult(result);
            DeviceManagement.restartProgrammingDevice(m, dest);
            throw new UpdaterException("Boot descriptor request failed.");
        }
        BootDescriptor bootDescriptor = BootDescriptor.fromArray(Arrays.copyOfRange(result, 4, result.length));
        logger.info("  Current firmware: {}", bootDescriptor);
        return bootDescriptor;
    }

    public static  String requestAppVersionString(UpdatableManagementClientImpl m, Destination dest)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        // byte[] result = m.sendUpdateData(dest, UPDCommand.APP_VERSION_REQUEST.id, new byte[] {0});
        byte[] result = sendWithRetry(m, dest, UPDCommand.APP_VERSION_REQUEST, new byte[] {0}, MAX_UPD_COMMAND_RETRY);
        if (result[3] != UPDCommand.APP_VERSION_RESPONSE.id){
            UPDProtocol.checkResult(result);
            //restartProgrammingDevice(mc, progDest);
            return null;
        }
        return new String(result,4,result.length - 4);	// Convert 12 bytes to string starting from result[4];
    }

    public static  void unlockDeviceWithUID(UpdatableManagementClientImpl m, Destination dest, byte[] uid)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nUnlocking device {} with UID {}...", dest.getAddress(), Utils.byteArrayToHex(uid));

        //byte[] result = m.sendUpdateData(dest, UPDCommand.UNLOCK_DEVICE.id, uid);
        byte[] result = sendWithRetry(m, dest, UPDCommand.UNLOCK_DEVICE, uid, MAX_UPD_COMMAND_RETRY);
        if (UPDProtocol.checkResult(result) != 0) {
            DeviceManagement.restartProgrammingDevice(m, dest);
            throw new UpdaterException("Selfbus update failed.");
        }
    }

    public static void eraseAddressRange(UpdatableManagementClientImpl mc, Destination pd, long startAddress, long totalLength)
            throws KNXLinkClosedException, InterruptedException, UpdaterException, KNXTimeoutException {
        long endAddress = startAddress + totalLength - 1;
        byte[] telegram = new byte[8];
        Utils.longToStream(telegram, 0 , startAddress);
        Utils.longToStream(telegram, 4 , endAddress);
        logger.info(String.format("Erasing firmware address range: 0x%04X - 0x%04X...", startAddress, endAddress));
        // byte[] result = mc.sendUpdateData(pd, UPDCommand.ERASE_ADDRESS_RANGE.id, telegram);
        byte[] result = sendWithRetry(mc, pd, UPDCommand.ERASE_ADDRESS_RANGE, telegram, MAX_UPD_COMMAND_RETRY);
        if (UPDProtocol.checkResult(result) != 0) {
            DeviceManagement.restartProgrammingDevice(mc, pd);
            throw new UpdaterException("Erasing firmware address range failed.");
        }
    }

    public static void eraseFlash(UpdatableManagementClientImpl mc, Destination pd)
            throws KNXLinkClosedException, InterruptedException, UpdaterException, KNXTimeoutException {
        // byte[] result = mc.sendUpdateData(pd, UPDCommand.ERASE_COMPLETE_FLASH.id, new byte[] {0});
        byte[] result = sendWithRetry(mc, pd, UPDCommand.ERASE_COMPLETE_FLASH, new byte[] {0}, MAX_UPD_COMMAND_RETRY);
        if (UPDProtocol.checkResult(result) != 0) {
            DeviceManagement.restartProgrammingDevice(mc, pd);
            throw new UpdaterException("Deleting the entire flash failed.");
        }
    }

    public static void dumpFlashRange(UpdatableManagementClientImpl mc, Destination pd, long startAddress, long endAddress)
            throws KNXLinkClosedException, InterruptedException, UpdaterException, KNXTimeoutException {
        byte[] telegram = new byte[8];
        Utils.longToStream(telegram, 0 , startAddress);
        Utils.longToStream(telegram, 4 , endAddress);
        // this send will always time out, because the mcu is busy dumping the flash
        sendWithRetry(mc, pd, UPDCommand.DUMP_FLASH, telegram, 0);
    }

    public static int doFlash(UpdatableManagementClientImpl mc, Destination pd, byte[] data, int maxRetry)
            throws UpdaterException, KNXLinkClosedException, InterruptedException, KNXTimeoutException {
        int nIndex = 0;
        while (nIndex < data.length)
        {
            byte[] txBuffer;
            if ((data.length - nIndex) >= Flash.MAX_PAYLOAD){
                txBuffer = new byte[Flash.MAX_PAYLOAD + 1];
            }
            else {
                txBuffer = new byte[data.length - nIndex + 1];
            }
            // First byte contains start address of following data
            txBuffer[0] = (byte)nIndex;
            System.arraycopy(data, nIndex, txBuffer, 1, txBuffer.length - 1);

            byte [] result = sendWithRetry(mc, pd, UPDCommand.SEND_DATA, txBuffer, maxRetry);
            if (UPDProtocol.checkResult(result, false) != 0) {
                DeviceManagement.restartProgrammingDevice(mc, pd);
                throw new UpdaterException("Selfbus update failed.");
            }

            nIndex += txBuffer.length - 1;
        }
        return nIndex;
    }

    public static byte[] sendWithRetry(UpdatableManagementClientImpl mc, Destination pd, UPDCommand command, byte[] data, int maxRetry)
            throws KNXLinkClosedException, InterruptedException, UpdaterException {
        while (true) {
            try {
                return mc.sendUpdateData(pd, command.id, data);
            }
            catch (KNXTimeoutException | KNXDisconnectException | KNXRemoteException e) {
                logger.warn("{}{} failed {}{}", ConColors.RED, command, e.getMessage(), ConColors.RESET);
                if (maxRetry > 0) {
                    maxRetry--;
                }

                if (maxRetry == 0)
                {
                    throw new UpdaterException(String.format("%s failed.", command));
                }
            }
        }
    }
}
