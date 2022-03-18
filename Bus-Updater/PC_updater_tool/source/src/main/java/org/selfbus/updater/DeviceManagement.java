package org.selfbus.updater;

import org.selfbus.updater.bootloader.BootDescriptor;
import org.selfbus.updater.bootloader.BootloaderIdentity;
import org.selfbus.updater.bootloader.BootloaderStatistic;
import org.selfbus.updater.upd.UPDCommand;
import org.selfbus.updater.upd.UPDProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.*;
import tuwien.auto.calimero.link.KNXLinkClosedException;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.mgmt.Destination;
import tuwien.auto.calimero.mgmt.KNXDisconnectException;

import java.time.Duration;
import java.util.Arrays;

import static org.selfbus.updater.Mcu.TL4_CONNECTION_TIMEOUT_MS;

/**
 * Provides methods to send firmware update telegrams to the bootloader (MCU)
 */
public final class DeviceManagement {
    private static final int RESTART_ERASE_CODE = 7; //!< EraseCode for the APCI_MASTER_RESET_PDU (valid from 1..7)
    private static final int RESTART_CHANNEL = 255;  //!< Channelnumber for the APCI_MASTER_RESET_PDU
    public static final int MAX_UPD_COMMAND_RETRY = 3; //!< default maximum retries a UPD command is sent to the client

    @SuppressWarnings("unused")
    private DeviceManagement (){}

    private final static Logger logger = LoggerFactory.getLogger(DeviceManagement.class.getName());
    private SBManagementClientImpl mc; //!< calimero device management client
    private Destination progDestination;
    private KNXNetworkLink link;

    public DeviceManagement(KNXNetworkLink link, IndividualAddress progDevice, int responseTimeoutSec, Priority priority)
            throws KNXLinkClosedException {
        this.link = link;
        logger.debug("Creating SBManagementClientImpl");
        this.mc = new SBManagementClientImpl(this.link);
        this.mc.responseTimeout(Duration.ofSeconds(responseTimeoutSec));
        this.mc.setPriority(priority);
        this.progDestination = this.mc.createDestination(progDevice, true, false, false);
    }

    public void restartProgrammingDevice()
            throws KNXTimeoutException, KNXLinkClosedException, InterruptedException {
        logger.warn("restarting device {}", progDestination);
        mc.restart(progDestination);
    }

    public void customRestartProgrammingDevice()
            throws KNXTimeoutException, KNXLinkClosedException, InterruptedException, UpdaterException {
        logger.warn("restarting device {}", progDestination);
        ResponseResult result = sendWithRetry(UPDCommand.RESET, new byte[]{0}, DeviceManagement.MAX_UPD_COMMAND_RETRY);  // Clean restart by application rather than lib
        if (UPDProtocol.checkResult(result.data()) != 0) {
            restartProgrammingDevice();
            throw new UpdaterException("Restarting device failed.");
        }
    }

    private void waitRestartTime(int restartTimeMs) {
        try {
            while (restartTimeMs > 0) {
                Thread.sleep(1000);
                System.out.printf("%s%s%s", ConColors.BRIGHT_GREEN, Utils.PROGRESS_MARKER, ConColors.RESET);
                restartTimeMs--;
            }
        }
        catch (final InterruptedException e) {
            logger.error("InterruptedException ", e);
        }
    }

    /**
     * Restarts the <code>device</code> in normal or Bootloader mode
     * @param link
     *          the KNX network link  to communicate with the device
     * @param device
     *          the IndividualAddress of the device to restart
     * @return true if successful, otherwise false
     */
    public boolean restartDeviceToBootloader(KNXNetworkLink link, IndividualAddress device)
            throws KNXLinkClosedException {
        SBManagementClientImpl mcDevice = new SBManagementClientImpl(link);
        Destination dest;
        dest = mcDevice.createDestination(device, true, false, false);
        int restartProcessTime = Mcu.DEFAULT_RESTART_TIME_SECONDS;
        try {
            logger.info("\nRestarting device {} into bootloader mode using {}", device, link);
            restartProcessTime =  mcDevice.restart(dest, RESTART_ERASE_CODE, RESTART_CHANNEL);
            mcDevice.detach();
            logger.info("Device {} reported {}{} seconds{} for restarting", device, ConColors.BRIGHT_GREEN, restartProcessTime, ConColors.RESET);
            waitRestartTime(restartProcessTime);
            System.out.println();
            logger.info("Device {} should now have started into bootloader mode.", device);
            return true;
        } catch (final KNXException | InterruptedException e) {
            logger.info("{}Restart state of device {} unknown. {}{}", ConColors.BRIGHT_RED, device, e.getMessage(), ConColors.RESET);
            logger.debug("KNXException ", e);
            logger.info("Waiting {}{} seconds{} for device {} to restart", ConColors.BRIGHT_GREEN, restartProcessTime, ConColors.RESET, device);
            waitRestartTime(restartProcessTime);
        } finally {
            mcDevice.detach();
        }
        return false;
    }

    public byte[] requestUIDFromDevice()
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nRequesting UID from {}...", progDestination.getAddress());
        //byte[] result = m.sendUpdateData(dest, UPDCommand.REQUEST_UID.id, new byte[0]);
        byte[] result = sendWithRetry(UPDCommand.REQUEST_UID, new byte[] {0}, MAX_UPD_COMMAND_RETRY).data();
        if (result[3] != UPDCommand.RESPONSE_UID.id) {
            UPDProtocol.checkResult(result, true);
            restartProgrammingDevice();
            throw new UpdaterException(String.format("Requesting UID failed! result[3]=0x%02X", result[3]));
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
            restartProgrammingDevice();
            throw new UpdaterException("Selfbus update failed.");
        }
    }

    public BootloaderIdentity requestBootloaderIdentity()
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nRequesting Bootloader Identity...");

        byte[] telegram = new byte[8];
        Utils.longToStream(telegram, 0 , ToolInfo.versionMajor());
        Utils.longToStream(telegram, 4 , ToolInfo.versionMinor());

        byte[] result = sendWithRetry(UPDCommand.REQUEST_BL_IDENTITY, telegram, MAX_UPD_COMMAND_RETRY).data();
        if (result[3] != UPDCommand.RESPONSE_BL_IDENTITY.id)
        {
            if (result[3] == UPDCommand.RESPONSE_BL_VERSION_MISMATCH.id) {
                long minMajorVersion = Utils.streamToLong(result, 4);
                long minMinorVersion = Utils.streamToLong(result, 8);
                logger.error("{}Selfbus Updater version {} not compatible. Please update to version {}.{} or higher.{}",
                        ConColors.RED, ToolInfo.getVersion(), minMajorVersion, minMinorVersion, ConColors.RESET);
            }
            else {
                UPDProtocol.checkResult(result);
            }
            restartProgrammingDevice();
            throw new UpdaterException(String.format("Requesting Bootloader Identity failed! result[3]=0x%02X", result[3]));
        }

        BootloaderIdentity bl = BootloaderIdentity.fromArray(Arrays.copyOfRange(result, 4, result.length));
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

    public BootDescriptor requestBootDescriptor()
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nRequesting Boot Descriptor...");
        //byte[] result = m.sendUpdateData(dest, UPDCommand.REQUEST_BOOT_DESC.id, new byte[] {0});
        byte[] result = sendWithRetry(UPDCommand.REQUEST_BOOT_DESC, new byte[] {0}, MAX_UPD_COMMAND_RETRY).data();
        if (result[3] != UPDCommand.RESPONSE_BOOT_DESC.id) {
            UPDProtocol.checkResult(result);
            restartProgrammingDevice();
            throw new UpdaterException(String.format("Boot descriptor request failed! result[3]=0x%02X", result[3]));
        }
        BootDescriptor bootDescriptor = BootDescriptor.fromArray(Arrays.copyOfRange(result, 4, result.length));
        logger.info("  Current firmware: {}", bootDescriptor);
        return bootDescriptor;
    }

    public String requestAppVersionString()
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        byte[] result = sendWithRetry(UPDCommand.APP_VERSION_REQUEST, new byte[] {0}, MAX_UPD_COMMAND_RETRY).data();
        if (result[3] != UPDCommand.APP_VERSION_RESPONSE.id){
            UPDProtocol.checkResult(result);
            return null;
        }
        return new String(result,4,result.length - 4);	// Convert 12 bytes to string starting from result[4];
    }

    public void unlockDeviceWithUID(byte[] uid)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nUnlocking device {} with UID {}...", progDestination.getAddress(), Utils.byteArrayToHex(uid));

        //byte[] result = m.sendUpdateData(dest, UPDCommand.UNLOCK_DEVICE.id, uid);
        byte[] result = sendWithRetry(UPDCommand.UNLOCK_DEVICE, uid, MAX_UPD_COMMAND_RETRY).data();
        if (UPDProtocol.checkResult(result) != 0) {
            restartProgrammingDevice();
            throw new UpdaterException("Selfbus update failed.");
        }
    }

    public void eraseAddressRange(long startAddress, long totalLength)
            throws KNXLinkClosedException, InterruptedException, UpdaterException, KNXTimeoutException {
        long endAddress = startAddress + totalLength - 1;
        byte[] telegram = new byte[8];
        Utils.longToStream(telegram, 0 , startAddress);
        Utils.longToStream(telegram, 4 , endAddress);
        logger.info(String.format("Erasing firmware address range: 0x%04X - 0x%04X...", startAddress, endAddress));
        // byte[] result = mc.sendUpdateData(pd, UPDCommand.ERASE_ADDRESS_RANGE.id, telegram);
        byte[] result = sendWithRetry(UPDCommand.ERASE_ADDRESS_RANGE, telegram, MAX_UPD_COMMAND_RETRY).data();
        if (UPDProtocol.checkResult(result) != 0) {
            restartProgrammingDevice();
            throw new UpdaterException("Erasing firmware address range failed.");
        }
    }

    public void eraseFlash()
            throws KNXLinkClosedException, InterruptedException, UpdaterException, KNXTimeoutException {
        // byte[] result = mc.sendUpdateData(pd, UPDCommand.ERASE_COMPLETE_FLASH.id, new byte[] {0});
        byte[] result = sendWithRetry(UPDCommand.ERASE_COMPLETE_FLASH, new byte[] {0}, MAX_UPD_COMMAND_RETRY).data();
        if (UPDProtocol.checkResult(result) != 0) {
            restartProgrammingDevice();
            throw new UpdaterException("Deleting the entire flash failed.");
        }
    }

    public void dumpFlashRange(long startAddress, long endAddress)
            throws KNXLinkClosedException, InterruptedException, UpdaterException, KNXTimeoutException {
        byte[] telegram = new byte[8];
        Utils.longToStream(telegram, 0 , startAddress);
        Utils.longToStream(telegram, 4 , endAddress);
        // sendWithRetry will always time out, because the mcu is busy dumping the flash
        sendWithRetry(UPDCommand.DUMP_FLASH, telegram, 0);
    }

    public ResponseResult doFlash(byte[] data, int maxRetry, int delay)
            throws UpdaterException, KNXLinkClosedException, InterruptedException, KNXTimeoutException {
        int nIndex = 0;
        ResponseResult result = new ResponseResult();
        while (nIndex < data.length)
        {

            byte[] txBuffer;
            if ((data.length - nIndex) >= Mcu.MAX_PAYLOAD){
                txBuffer = new byte[Mcu.MAX_PAYLOAD + 1];
            }
            else {
                txBuffer = new byte[data.length - nIndex + 1];
            }
            // First byte contains start address of following data
            txBuffer[0] = (byte)nIndex;
            System.arraycopy(data, nIndex, txBuffer, 1, txBuffer.length - 1);

            ResponseResult tmp = sendWithRetry(UPDCommand.SEND_DATA, txBuffer, maxRetry);
            result.addCounters(tmp);

            if ((tmp.dropCount() > 0) || (tmp.timeoutCount() > 0)) {
                logger.warn("{}x{}", ConColors.RED, ConColors.RESET);
                continue;
            }

            if (UPDProtocol.checkResult(tmp.data(), false) != 0) {
                restartProgrammingDevice();
                throw new UpdaterException("doFlash failed.");
            }

            if (delay > 0) {
                Thread.sleep(delay); //Reduce bus load during data upload, without 2:04, 50ms 2:33, 60ms 2:41, 70ms 2:54, 80ms 3:04
            }

            nIndex += txBuffer.length - 1;
        }
        result.addWritten(nIndex);
        return result;
    }

    public ResponseResult programBootDescriptor(BootDescriptor bootDescriptor, int delay)
            throws UpdaterException, KNXLinkClosedException, InterruptedException, KNXTimeoutException {

        byte[] streamBootDescriptor = bootDescriptor.toStream();
        // send new boot descriptor
        ResponseResult flashResult = doFlash(streamBootDescriptor, -1, delay);
        if (flashResult.written() != streamBootDescriptor.length) {
            throw new UpdaterException(String.format("Sending Boot descriptor (length %d) failed. Wrote %d", streamBootDescriptor.length, flashResult.written()));
        }
        if (delay > 0) {
            Thread.sleep(delay);
        }
        int crc32Value = Utils.crc32Value(streamBootDescriptor);
        byte[] programBootDescriptor = new byte[9];
        Utils.longToStream(programBootDescriptor, 0, streamBootDescriptor.length);
        Utils.longToStream(programBootDescriptor, 4, crc32Value);
        System.out.println();
        logger.info("Updating boot descriptor with CRC32 0x{}, length {}",
                Integer.toHexString(crc32Value), streamBootDescriptor.length);
        ResponseResult programResult = sendWithRetry(UPDCommand.UPDATE_BOOT_DESC, programBootDescriptor, DeviceManagement.MAX_UPD_COMMAND_RETRY);
        if (UPDProtocol.checkResult(programResult.data()) != 0) {
            restartProgrammingDevice();
            throw new UpdaterException("Updating boot descriptor failed.");
        }
        programResult.addCounters(flashResult);
        if (delay > 0) {
            Thread.sleep(delay);
        }
        return programResult;
    }

    public void requestBootLoaderStatistic()
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {

        byte[] result = sendWithRetry(UPDCommand.REQUEST_STATISTIC, new byte[] {0}, MAX_UPD_COMMAND_RETRY).data();
        if (result[3] == UPDCommand.RESPONSE_STATISTIC.id)
        {
            BootloaderStatistic blStatistic = BootloaderStatistic.fromArray(Arrays.copyOfRange(result, 4, result.length));
            logger.info("  {}{}{}", ConColors.BRIGHT_YELLOW, blStatistic, ConColors.RESET);
        }
        else {
            logger.warn("  {}{}{}", ConColors.RED, String.format("Requesting Bootloader statistic failed! result[3]=0x%02X", result[3]), ConColors.RESET);
        }
    }

    public ResponseResult sendWithRetry(UPDCommand command, byte[] data, int maxRetry)
            throws UpdaterException, InterruptedException {
        ResponseResult result = new ResponseResult();
        while (true) {
            try {
                byte[] data2 = mc.sendUpdateData(progDestination, command.id, data);
                result.copyFromArray(data2);
                return result;
            }
            catch (KNXTimeoutException | KNXRemoteException e) {
                logger.warn("{}{} {} : {}{}", ConColors.RED, command, e.getMessage(), e.getClass().getSimpleName(), ConColors.RESET);
                result.incTimeoutCount();
                Thread.sleep(TL4_CONNECTION_TIMEOUT_MS);
            }
            catch (KNXDisconnectException e) {
                logger.warn("{}{} {} : {}{}", ConColors.RED, command, e.getMessage(), e.getClass().getSimpleName(), ConColors.RESET);
                result.incDropCount();
                Thread.sleep(TL4_CONNECTION_TIMEOUT_MS);
            }
            catch (KNXIllegalArgumentException e) {
                throw new UpdaterException(String.format("%s failed.", command), e);
            }
            catch (Throwable e) {
                throw new UpdaterException(String.format("%s failed.", command), e);
                // logger.error("{}{} Exception {}{}", ConColors.RED, command, e, ConColors.RESET);
            }

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
