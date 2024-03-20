package org.selfbus.updater;

import org.selfbus.updater.bootloader.BootDescriptor;
import org.selfbus.updater.bootloader.BootloaderIdentity;
import org.selfbus.updater.bootloader.BootloaderStatistic;
import org.selfbus.updater.upd.UDPProtocolVersion;
import org.selfbus.updater.upd.UDPResult;
import org.selfbus.updater.upd.UPDCommand;
import org.selfbus.updater.upd.UPDProtocol;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import tuwien.auto.calimero.*;
import tuwien.auto.calimero.link.KNXLinkClosedException;
import tuwien.auto.calimero.link.KNXNetworkLink;
import tuwien.auto.calimero.mgmt.Destination;
import tuwien.auto.calimero.mgmt.KNXDisconnectException;
import tuwien.auto.calimero.mgmt.ManagementProcedures;
import tuwien.auto.calimero.mgmt.ManagementProceduresImpl;

import java.time.Duration;
import java.util.Arrays;

import static org.selfbus.updater.Mcu.MAX_FLASH_ERASE_TIMEOUT;
import static org.selfbus.updater.upd.UPDProtocol.COMMAND_POSITION;
import static org.selfbus.updater.upd.UPDProtocol.DATA_POSITION;

/**
 * Provides methods to send firmware update telegrams to the bootloader (MCU)
 */
public final class DeviceManagement {
    private static final int RESTART_ERASE_CODE = 7; //!< EraseCode for the APCI_MASTER_RESET_PDU (valid from 1..7)
    private static final int RESTART_CHANNEL = 255;  //!< Channelnumber for the APCI_MASTER_RESET_PDU

    private static final int MAX_UPD_COMMAND_RETRY = 3; //!< default maximum retries a UPD command is sent to the client

    private UDPProtocolVersion protocolVersion;

    private int blockSize;
    private int maxPayload;
    private int updSendDataOffset;

    private DeviceManagement () {
        setProtocolVersion(UDPProtocolVersion.UDP_V1);
    }

    private final static Logger logger = LoggerFactory.getLogger(DeviceManagement.class.getName());
    private SBManagementClientImpl mc; //!< calimero device management client
    private Destination progDestination;
    private KNXNetworkLink link;

    public DeviceManagement(KNXNetworkLink link, IndividualAddress progDevice, Priority priority)
            throws KNXLinkClosedException {
        this();
        this.link = link;
        logger.debug("Creating SBManagementClientImpl");
        this.mc = new SBManagementClientImpl(this.link);
        this.mc.setPriority(priority);
        this.progDestination = this.mc.createDestination(progDevice, true, false, false);
    }

    public void restartProgrammingDevice()
            throws KNXTimeoutException, KNXLinkClosedException, InterruptedException {
        logger.info("Restarting device {}", progDestination);
        mc.restart(progDestination);
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
     * Restarts the <code>device</code> running in normal into the Bootloader mode
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
            logger.info("Device {} reported {}{} second(s){} for restarting", device, ConColors.BRIGHT_GREEN, restartProcessTime, ConColors.RESET);
            waitRestartTime(restartProcessTime);
            System.out.println();
            return true;
        } catch (final KNXException | InterruptedException e) {
            logger.info("{}Restart state of device {} unknown. {}{}", ConColors.BRIGHT_RED, device, e.getMessage(), ConColors.RESET);
            logger.debug("KNXException ", e);
            logger.info("Waiting {}{} seconds{} for device {} to restart", ConColors.BRIGHT_GREEN, restartProcessTime, ConColors.RESET, device);
            waitRestartTime(restartProcessTime);
        } finally {
            mcDevice.detach();
            mcDevice.close();
        }
        return false;
    }

    public byte[] requestUIDFromDevice()
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nRequesting UID from {}...", progDestination.getAddress());
        byte[] result = sendWithRetry(UPDCommand.REQUEST_UID, new byte[0], MAX_UPD_COMMAND_RETRY).data();
        if (result[COMMAND_POSITION] != UPDCommand.RESPONSE_UID.id) {
            UPDProtocol.checkResult(result, true);
            restartProgrammingDevice();
            throw new UpdaterException(String.format("Requesting UID failed! result[%d]=0x%02X", COMMAND_POSITION, result[COMMAND_POSITION]));
        }

        byte[] uid;
        if ((result.length >= UPDProtocol.UID_LENGTH_USED) && (result.length <= UPDProtocol.UID_LENGTH_MAX)){
            uid = Arrays.copyOfRange(result, DATA_POSITION, UPDProtocol.UID_LENGTH_USED + DATA_POSITION);
            logger.info("  got: {} length {}", Utils.byteArrayToHex(uid), uid.length);
            return uid;
        } else {
            uid = Arrays.copyOfRange(result, DATA_POSITION, result.length - DATA_POSITION);
            logger.error("Request UID failed {} result.length={}, UID_LENGTH_USED={}, UID_LENGTH_MAX={}",
                    Utils.byteArrayToHex(uid),uid.length, UPDProtocol.UID_LENGTH_USED, UPDProtocol.UID_LENGTH_MAX);
            restartProgrammingDevice();
            throw new UpdaterException("Selfbus update failed.");
        }
    }

    public BootloaderIdentity requestBootloaderIdentity()
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nRequesting Bootloader Identity...");

        byte[] telegram = new byte[2];
        telegram[0] = (byte) ToolInfo.versionMajor();
        telegram[1] = (byte) ToolInfo.versionMinor();

        byte[] result = sendWithRetry(UPDCommand.REQUEST_BL_IDENTITY, telegram, MAX_UPD_COMMAND_RETRY).data();
        if (result[COMMAND_POSITION] != UPDCommand.RESPONSE_BL_IDENTITY.id)
        {
            if (result[COMMAND_POSITION] == UPDCommand.RESPONSE_BL_VERSION_MISMATCH.id) {
                long minMajorVersion = result[DATA_POSITION] & 0xff;
                long minMinorVersion = result[DATA_POSITION + 1] & 0xff;
                logger.error("{}Selfbus Updater version {} is not compatible. Please update to version {}.{} or higher.{}",
                        ConColors.RED, ToolInfo.getVersion(), minMajorVersion, minMinorVersion, ConColors.RESET);
            }
            else {
                UPDProtocol.checkResult(result);
            }
            restartProgrammingDevice();
            throw new UpdaterException("Requesting Bootloader Identity failed!");
        }

        BootloaderIdentity bl = BootloaderIdentity.fromArray(Arrays.copyOfRange(result, DATA_POSITION, result.length));
        logger.info("  Device Bootloader: {}{}{}", ConColors.BRIGHT_YELLOW, bl, ConColors.RESET);

        boolean versionsMatch = (bl.getVersionMajor() > ToolInfo.minMajorVersionBootloader()) ||
                ((bl.getVersionMajor() == ToolInfo.minMajorVersionBootloader()) && (bl.getVersionMinor() >= ToolInfo.minMinorVersionBootloader()));

        if (!versionsMatch)
        {
            logger.error("{}Bootloader version {} is not compatible, please update Bootloader to version {} or higher{}",
                    ConColors.RED, bl.getVersion(), ToolInfo.minVersionBootloader(), ConColors.RESET);
            throw new UpdaterException("Bootloader version not compatible!");
        }
        return bl;
    }

    public BootDescriptor requestBootDescriptor()
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("Requesting Boot Descriptor...");
        byte[] result = sendWithRetry(UPDCommand.REQUEST_BOOT_DESC, new byte[0], MAX_UPD_COMMAND_RETRY).data();
        if (result[COMMAND_POSITION] != UPDCommand.RESPONSE_BOOT_DESC.id) {
            UPDProtocol.checkResult(result);
            restartProgrammingDevice();
            throw new UpdaterException(String.format("Boot descriptor request failed! result[%d]=0x%02X, result[%d]=0x%02X",
                    COMMAND_POSITION, result[COMMAND_POSITION],
                    DATA_POSITION, result[DATA_POSITION]));
        }
        BootDescriptor bootDescriptor = BootDescriptor.fromArray(Arrays.copyOfRange(result, DATA_POSITION, result.length));
        logger.info("  Current firmware: {}", bootDescriptor);
        return bootDescriptor;
    }

    public String requestAppVersionString()
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        byte[] result = sendWithRetry(UPDCommand.APP_VERSION_REQUEST, new byte[0], MAX_UPD_COMMAND_RETRY).data();
        if (result[COMMAND_POSITION] != UPDCommand.APP_VERSION_RESPONSE.id){
            UPDProtocol.checkResult(result);
            return null;
        }
        return new String(result,DATA_POSITION,result.length - DATA_POSITION);	// Convert 12 bytes to string starting from result[DATA_POSITION];
    }

    public void unlockDeviceWithUID(byte[] uid)
            throws KNXTimeoutException, KNXLinkClosedException, KNXDisconnectException, KNXRemoteException, InterruptedException, UpdaterException {
        logger.info("\nUnlocking device {} with UID {}...", progDestination.getAddress(), Utils.byteArrayToHex(uid));
        byte[] result = sendWithRetry(UPDCommand.UNLOCK_DEVICE, uid, MAX_UPD_COMMAND_RETRY).data();
        if (UPDProtocol.checkResult(result) != UDPResult.IAP_SUCCESS.id) {
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
        Duration oldResponseTimeout = mc.responseTimeout();
        Duration newResponseTimeout = MAX_FLASH_ERASE_TIMEOUT.multipliedBy(2);
        if (oldResponseTimeout.compareTo(newResponseTimeout) < 0) {
            mc.responseTimeout(newResponseTimeout); // temporarily increase responseTimeout
            logger.trace("mc.ResponseTimeout temporarily increased to {}", mc.responseTimeout());
        }

        byte[] result = sendWithRetry(UPDCommand.ERASE_ADDRESS_RANGE, telegram, MAX_UPD_COMMAND_RETRY).data();

        mc.responseTimeout(oldResponseTimeout); // restore responseTimeout
        logger.trace("mc.ResponseTimeout restored to {}", mc.responseTimeout());

        if (UPDProtocol.checkResult(result) != UDPResult.IAP_SUCCESS.id) {
            restartProgrammingDevice();
            throw new UpdaterException("Erasing firmware address range failed.");
        }
    }

    public void eraseFlash()
            throws KNXLinkClosedException, InterruptedException, UpdaterException, KNXTimeoutException {
        byte[] result = sendWithRetry(UPDCommand.ERASE_COMPLETE_FLASH, new byte[0], MAX_UPD_COMMAND_RETRY).data();
        if (UPDProtocol.checkResult(result) != UDPResult.IAP_SUCCESS.id) {
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
        byte[] result = sendWithRetry(UPDCommand.DUMP_FLASH, telegram, 0).data();
        if (UPDProtocol.checkResult(result) != UDPResult.IAP_SUCCESS.id) {
            restartProgrammingDevice();
            throw new UpdaterException("Flash dumping failed.");
        }
    }

    public ResponseResult doFlash(byte[] data, int maxRetry, int delay)
            throws UpdaterException, KNXLinkClosedException, InterruptedException, KNXTimeoutException {
        int nIndex = 0;
        ResponseResult result = new ResponseResult();
        while (nIndex < data.length)
        {
            byte[] txBuffer;
            if ((data.length + updSendDataOffset - nIndex) >= maxPayload) {
                txBuffer = new byte[maxPayload];
            }
            else {
                txBuffer = new byte[data.length + updSendDataOffset- nIndex];
            }

            if (protocolVersion == UDPProtocolVersion.UDP_V0) {
                txBuffer[0] = (byte) nIndex; // First byte contains mcu's ramBuffer start position to copy to
            }
            System.arraycopy(data, nIndex, txBuffer, updSendDataOffset, txBuffer.length - updSendDataOffset);

            ResponseResult tmp = sendWithRetry(UPDCommand.SEND_DATA, txBuffer, maxRetry);
            result.addCounters(tmp);

            if ((tmp.dropCount() > 0) || (tmp.timeoutCount() > 0)) {
                logger.warn("{}x{}", ConColors.RED, ConColors.RESET);
                continue;
            }

            if (UPDProtocol.checkResult(tmp.data(), false) != UDPResult.IAP_SUCCESS.id) {
                restartProgrammingDevice();
                throw new UpdaterException("doFlash failed.");
            }

            nIndex += txBuffer.length - updSendDataOffset;

            if (delay > 0) {
                Thread.sleep(delay); //Reduce bus load during data upload, without 2:04, 50ms 2:33, 60ms 2:41, 70ms 2:54, 80ms 3:04
            }
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
        byte[] programBootDescriptor = new byte[8];
        Utils.longToStream(programBootDescriptor, 0, streamBootDescriptor.length);
        Utils.longToStream(programBootDescriptor, 4, crc32Value);
        System.out.println();
        logger.info("Updating boot descriptor with CRC32 0x{}, length {}",
                Integer.toHexString(crc32Value), streamBootDescriptor.length);
        ResponseResult programResult = sendWithRetry(UPDCommand.UPDATE_BOOT_DESC, programBootDescriptor, DeviceManagement.MAX_UPD_COMMAND_RETRY);
        if (UPDProtocol.checkResult(programResult.data()) != UDPResult.IAP_SUCCESS.id) {
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

        byte[] result = sendWithRetry(UPDCommand.REQUEST_STATISTIC, new byte[0], MAX_UPD_COMMAND_RETRY).data();
        if (result[COMMAND_POSITION] == UPDCommand.RESPONSE_STATISTIC.id)
        {
            BootloaderStatistic blStatistic = BootloaderStatistic.fromArray(Arrays.copyOfRange(result, DATA_POSITION, result.length));
            logger.info("  {}{}{}", ConColors.BRIGHT_YELLOW, blStatistic, ConColors.RESET);
        }
        else {
            logger.warn("  {}{}{}", ConColors.RED,
                    String.format("Requesting Bootloader statistic failed! result[%d]=0x%02X, result[%d]=0x%02X",
                            COMMAND_POSITION, result[COMMAND_POSITION],
                            DATA_POSITION, result[DATA_POSITION]), ConColors.RESET);
        }
    }

    public ResponseResult sendWithRetry(UPDCommand command, byte[] data, int maxRetry)
            throws UpdaterException {
        ResponseResult result = new ResponseResult();
        while (true) {
            try {
                byte[] data2 = mc.sendUpdateData(progDestination, command.id, data);
                result.copyFromArray(data2);
                return result;
            }
            catch (KNXTimeoutException e) {
                logger.warn("{}{} {} : {}{}", ConColors.RED, command, e.getMessage(), e.getClass().getSimpleName(), ConColors.RESET);
                result.incTimeoutCount();
            }
            catch (KNXDisconnectException | KNXRemoteException e) { ///\todo check causes of KNXRemoteException, if think they are unrecoverable
                logger.warn("{}{} {} : {}{}", ConColors.RED, command, e.getMessage(), e.getClass().getSimpleName(), ConColors.RESET);
                result.incDropCount();
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

    public void checkDeviceInProgrammingMode(IndividualAddress progDeviceAddr) throws UpdaterException {
        try {
            ManagementProcedures mgmt = new ManagementProceduresImpl(link);
            IndividualAddress[] devices = mgmt.readAddress();
            mgmt.detach();
            mgmt.close();
            if ((devices.length == 0) && (progDeviceAddr == null)) { // no device in prog mode
            	return;
            }
            else if ((devices.length == 1) && (progDeviceAddr != null) && (progDeviceAddr.equals(devices[0]))) { // correct device in prog mode
            	return;
            }
            logger.warn("{}{} Device(s) in bootloader/programming mode: {}{}", ConColors.BRIGHT_RED, devices.length, Arrays.stream(devices).toArray(), ConColors.RESET);
            if (devices.length == 0) {
                throw new UpdaterException("No device in programming mode.");
            }
            else {
                throw new UpdaterException(String.format("%d wrong device(s) %s are already in programming mode.", devices.length, Arrays.toString(devices)));
            }
        } catch (KNXException | InterruptedException e ) {
            throw new UpdaterException(String.format("checkDevicesInProgrammingMode failed. %s", e.getMessage()));
        }
    }

    public UDPProtocolVersion getProtocolVersion() {
        return protocolVersion;
    }

    public void setProtocolVersion(UDPProtocolVersion protocolVersion) {
        this.protocolVersion = protocolVersion;
        this.maxPayload = Mcu.MAX_PAYLOAD;
        if (this.protocolVersion == UDPProtocolVersion.UDP_V1) {
            this.blockSize = Mcu.UPD_PROGRAM_SIZE;
            this.updSendDataOffset = 0;
        }
        else {
            this.blockSize = Mcu.FLASH_PAGE_SIZE;
            this.updSendDataOffset = 1;
        }
    }

    public int getBlockSize() {
        return blockSize;
    }

    public boolean setBlockSize(int blockSize) {
        if (this.protocolVersion == UDPProtocolVersion.UDP_V0) {
            return false;
        }
        this.blockSize = blockSize;
        return true;
    }

    public int getMaxPayload() {
        return maxPayload;
    }
}
