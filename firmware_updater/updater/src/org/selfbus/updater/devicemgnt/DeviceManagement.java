package org.selfbus.updater.devicemgnt;

import org.selfbus.updater.*;
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

import java.net.UnknownHostException;
import java.time.Duration;
import java.util.Arrays;

import static org.fusesource.jansi.Ansi.*;
import static org.fusesource.jansi.Ansi.Color.*;
import static org.selfbus.updater.Mcu.MAX_FLASH_ERASE_TIMEOUT;
import static org.selfbus.updater.logging.Markers.CONSOLE_GUI_ONLY;
import static org.selfbus.updater.logging.Markers.CONSOLE_GUI_NO_NEWLINE;
import static org.selfbus.updater.upd.UPDProtocol.DATA_POSITION;

/**
 * Provides methods to send firmware update telegrams to the bootloader (MCU)
 */
public class DeviceManagement implements AutoCloseable {
    /**
     * EraseCode for the APCI_MASTER_RESET_PDU (valid from 1..7)
     */
    private static final int RESTART_ERASE_CODE = 7;
    /**
     * Channelnumber for the APCI_MASTER_RESET_PDU
     */
    private static final int RESTART_CHANNEL = 255;

    /**
     * Default maximum retries a UPD command is sent to the client
     */
    private static final int MAX_UPD_COMMAND_RETRY = 3;

    private UDPProtocolVersion protocolVersion;

    private int blockSize;
    private int maxPayload;
    private int updSendDataOffset;

    private final static Logger logger = LoggerFactory.getLogger(DeviceManagement.class);
    /**
     * Calimero device management client
     */
    private SBManagementClientImpl mc;
    private Destination progDestination;
    protected KNXNetworkLink link;

    private IndividualAddress progDevice;

    protected CliOptions cliOptions;

    private DeviceManagement () {
        setProtocolVersion(UDPProtocolVersion.UDP_V1);
    }

    public DeviceManagement(CliOptions cliOptions) {
        this();
        this.cliOptions = cliOptions;
    }

    public void reconnect(int waitMs) throws KNXException, UpdaterException, UnknownHostException, InterruptedException {
        close();
        if (waitMs <= 0) {
            waitMs = 100;
        }
        logger.info("Reconnecting in {} ms", waitMs);
        Thread.sleep(waitMs);
        open();
    }

    public void open() throws KNXException, UpdaterException, UnknownHostException, InterruptedException {
        close();
        this.link = new SBKNXLink(this.cliOptions).openLink();
        this.progDevice = cliOptions.getProgDevicePhysicalAddress();
        this.mc = new SBManagementClientImpl(this.link);
        this.mc.setPriority(cliOptions.getPriority());
        this.progDestination = this.mc.createDestination(progDevice, true, false, false);
    }

    @Override
    public void close() {
        logger.debug("Closing {}", this.getClass().getSimpleName());
        if (progDestination != null) {
            logger.debug("Releasing progDestination {}", progDestination);
            progDestination.close();
        }

        if (mc != null) {
            logger.debug("Releasing mc {}", mc);
            mc.close(); // mc.close calls already mc.detach()
        }
        if (link != null) {
            logger.debug("Releasing link {}", link);
            link.close();
        }
        progDestination = null;
        progDevice = null;
        mc = null;
        link = null;
    }

    public void restartProgrammingDevice()
            throws KNXTimeoutException, KNXLinkClosedException, InterruptedException {
        logger.info("Restarting device {}", progDestination);
        mc.restart(progDestination);
    }

    private void waitRestartTime(int restartTimeSeconds) throws InterruptedException {
        while (restartTimeSeconds > 0) {
            Thread.sleep(1000);
            logger.info(CONSOLE_GUI_NO_NEWLINE, String.format("%s.%s", ansi().fgBright(GREEN), ansi().reset()));
            restartTimeSeconds--;
        }
        logger.info(CONSOLE_GUI_ONLY, ""); // Just a new line
    }

    /**
     * Restarts the <code>device</code> running in normal into the Bootloader mode
     *
     * @param device the IndividualAddress of the device to restart
     */
    public void restartDeviceToBootloader(IndividualAddress device) throws InterruptedException {
        int restartProcessTime = Mcu.DEFAULT_RESTART_TIME_SECONDS;
        try (Destination dest = this.mc.createDestination(device, true, false, false)) {
            logger.info("Restarting device {} into bootloader", device);
            restartProcessTime = this.mc.restart(dest, RESTART_ERASE_CODE, RESTART_CHANNEL);
            logger.info("Device {} reported {}{}{} second(s) for restarting",
                    device, ansi().fgBright(GREEN), restartProcessTime, ansi().reset());
            waitRestartTime(restartProcessTime);
        } catch (final KNXException e) {
            logger.info("{}Restart state of device {} unknown. {}{}",
                    ansi().fgBright(RED), device, e.getMessage(), ansi().reset());
            logger.debug("", e); // todo see logback issue https://github.com/qos-ch/logback/issues/876
            logger.info("Waiting {}{}{} seconds for device {} to restart",
                    ansi().fgBright(GREEN), restartProcessTime, ansi().reset(), device);
            waitRestartTime(restartProcessTime);
        }
    }

    public String requestUIDFromDevice()
            throws KNXTimeoutException, KNXLinkClosedException, InterruptedException, UpdaterException {
        logger.info("Requesting UID from {}", progDestination.getAddress());
        byte[] result = sendWithRetry(UPDCommand.REQUEST_UID, new byte[0], getMaxUpdCommandRetry()).data();
        UPDCommand command = UPDCommand.tryFromByteArray(result);
        if (command != UPDCommand.RESPONSE_UID) {
            restartProgrammingDevice();
            throw new UpdaterException("Requesting UID failed!");
        }

        byte[] uid;
        if ((result.length >= UPDProtocol.UID_LENGTH_USED) && (result.length <= UPDProtocol.UID_LENGTH_MAX)){
            uid = Arrays.copyOfRange(result, DATA_POSITION, UPDProtocol.UID_LENGTH_USED + DATA_POSITION);
            logger.info("  got: {} length {}", UPDProtocol.byteArrayToHex(uid), uid.length);
            return UPDProtocol.byteArrayToHex(uid);
        } else {
            uid = Arrays.copyOfRange(result, DATA_POSITION, result.length - DATA_POSITION);
            String errorMsg = String.format("Request UID failed %s result.length=%d, UID_LENGTH_USED=%d, UID_LENGTH_MAX=%d",
                    UPDProtocol.byteArrayToHex(uid), uid.length, UPDProtocol.UID_LENGTH_USED, UPDProtocol.UID_LENGTH_MAX);
            logger.error(errorMsg);
            restartProgrammingDevice();
            throw new UpdaterException(errorMsg);
        }
    }

    public BootloaderIdentity requestBootloaderIdentity()
            throws KNXTimeoutException, KNXLinkClosedException, InterruptedException, UpdaterException {
        logger.debug("Requesting Bootloader Identity");

        byte[] telegram = new byte[2];
        telegram[0] = (byte) ToolInfo.versionMajor();
        telegram[1] = (byte) ToolInfo.versionMinor();

        byte[] result = sendWithRetry(UPDCommand.REQUEST_BL_IDENTITY, telegram, getMaxUpdCommandRetry()).data();
        UPDCommand command = UPDCommand.tryFromByteArray(result);
        if (command != UPDCommand.RESPONSE_BL_IDENTITY)
        {
            if (command == UPDCommand.RESPONSE_BL_VERSION_MISMATCH) {
                long minMajorVersion = result[DATA_POSITION] & 0xff;
                long minMinorVersion = result[DATA_POSITION + 1] & 0xff;
                logger.error("{}Selfbus Updater version {} is not compatible. Please update to version {}.{} or higher.{}",
                        ansi().fg(RED), ToolInfo.getVersion(), minMajorVersion, minMinorVersion, ansi().reset());
            }
            restartProgrammingDevice();
            throw new UpdaterException("Requesting Bootloader Identity failed!");
        }

        BootloaderIdentity bl = BootloaderIdentity.fromArray(Arrays.copyOfRange(result, DATA_POSITION, result.length));
        logger.info("Device Bootloader: {}{}{}", ansi().fgBright(YELLOW), bl, ansi().reset());

        boolean versionsMatch = (bl.versionMajor() > ToolInfo.minMajorVersionBootloader()) ||
                ((bl.versionMajor() == ToolInfo.minMajorVersionBootloader()) && (bl.versionMinor() >= ToolInfo.minMinorVersionBootloader()));

        if (!versionsMatch)
        {
            logger.error("{}Bootloader version {} is not compatible, please update Bootloader to version {} or higher{}",
                    ansi().fg(RED), bl.getVersion(), ToolInfo.minVersionBootloader(), ansi().reset());
            throw new UpdaterException("Bootloader version not compatible!");
        }
        return bl;
    }

    public BootDescriptor requestBootDescriptor()
            throws KNXTimeoutException, KNXLinkClosedException, InterruptedException, UpdaterException {
        logger.debug("Requesting Boot Descriptor");
        byte[] result = sendWithRetry(UPDCommand.REQUEST_BOOT_DESC, new byte[0], getMaxUpdCommandRetry()).data();
        UPDCommand command = UPDCommand.tryFromByteArray(result);
        if (command != UPDCommand.RESPONSE_BOOT_DESC) {
            restartProgrammingDevice();
            throw new UpdaterException("Boot descriptor request failed!");
        }

        BootDescriptor bootDescriptor = BootDescriptor.fromArray(Arrays.copyOfRange(result, DATA_POSITION, result.length));
        logger.info("Current firmware: {}", bootDescriptor);
        return bootDescriptor;
    }

    public String requestAppVersionString() throws UpdaterException,
            InterruptedException {
        byte[] result = sendWithRetry(UPDCommand.APP_VERSION_REQUEST, new byte[0], getMaxUpdCommandRetry()).data();
        UPDCommand command = UPDCommand.tryFromByteArray(result);
        if (command != UPDCommand.APP_VERSION_RESPONSE) {
            return "";
        }

        return new String(result,DATA_POSITION,result.length - DATA_POSITION);	// Convert 12 bytes to string starting from result[DATA_POSITION];
    }

    public void unlockDeviceWithUID(String uid)
            throws KNXTimeoutException, KNXLinkClosedException, InterruptedException, UpdaterException {
        logger.info("Unlocking device {} with UID {}", progDestination.getAddress(), uid);
        byte[] result = sendWithRetry(UPDCommand.UNLOCK_DEVICE, UPDProtocol.uidToByteArray(uid), getMaxUpdCommandRetry()).data();
        if (UPDProtocol.checkResult(result) != UDPResult.IAP_SUCCESS) {
            restartProgrammingDevice();
            throw new UpdaterException(String.format("Unlocking device %s failed.", progDestination.getAddress()));
        }
    }

    public void eraseAddressRange(long startAddress, long totalLength)
            throws KNXLinkClosedException, InterruptedException, UpdaterException, KNXTimeoutException {
        long endAddress = startAddress + totalLength - 1;
        byte[] telegram = new byte[8];
        Utils.longToStream(telegram, 0 , startAddress);
        Utils.longToStream(telegram, 4 , endAddress);
        logger.info("Erasing firmware address range: {}", String.format("0x%04X-0x%04X", startAddress, endAddress));
        Duration oldResponseTimeout = mc.responseTimeout();
        Duration newResponseTimeout = MAX_FLASH_ERASE_TIMEOUT.multipliedBy(2);
        if (oldResponseTimeout.compareTo(newResponseTimeout) < 0) {
            mc.responseTimeout(newResponseTimeout); // temporarily increase responseTimeout
            logger.trace("mc.ResponseTimeout temporarily increased to {}", mc.responseTimeout());
        }

        byte[] result = sendWithRetry(UPDCommand.ERASE_ADDRESS_RANGE, telegram, getMaxUpdCommandRetry()).data();

        mc.responseTimeout(oldResponseTimeout); // restore responseTimeout
        logger.trace("mc.ResponseTimeout restored to {}", mc.responseTimeout());

        if (UPDProtocol.checkResult(result) != UDPResult.IAP_SUCCESS) {
            restartProgrammingDevice();
            throw new UpdaterException("Erasing firmware address range failed.");
        }
    }

    public void eraseFlash()
            throws KNXLinkClosedException, InterruptedException, UpdaterException, KNXTimeoutException {
        byte[] result = sendWithRetry(UPDCommand.ERASE_COMPLETE_FLASH, new byte[0], getMaxUpdCommandRetry()).data();
        if (UPDProtocol.checkResult(result) != UDPResult.IAP_SUCCESS) {
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
        if (UPDProtocol.checkResult(result) != UDPResult.IAP_SUCCESS) {
            restartProgrammingDevice();
            throw new UpdaterException("Flash dumping failed.");
        }
    }

    public ResponseResult doFlash(byte[] data, int maxRetry, int delay, ProgressInfo progressInfo)
            throws UpdaterException, InterruptedException {
        int nIndex = 0;
        ResponseResult result = new ResponseResult();
        while (nIndex < data.length)
        {
            byte[] txBuffer;
            if ((data.length + updSendDataOffset - nIndex) >= getMaxPayload()) {
                txBuffer = new byte[getMaxPayload()];
            }
            else {
                txBuffer = new byte[data.length + updSendDataOffset - nIndex];
            }

            if (getProtocolVersion() == UDPProtocolVersion.UDP_V0) {
                txBuffer[0] = (byte) nIndex; // First byte contains mcu's ramBuffer start position to copy to
            }
            System.arraycopy(data, nIndex, txBuffer, updSendDataOffset, txBuffer.length - updSendDataOffset);

            ResponseResult tmp = sendWithRetry(UPDCommand.SEND_DATA, txBuffer, maxRetry);
            result.addCounters(tmp);

            if (UPDProtocol.checkResult(tmp.data(), false) != UDPResult.IAP_SUCCESS) {
                throw new UpdaterException("doFlash failed.");
            }
            updateProgressInfo(progressInfo, txBuffer.length - updSendDataOffset);
            nIndex += txBuffer.length - updSendDataOffset;

            if (delay > 0) {
                Thread.sleep(delay); //Reduce bus load during data upload, without 2:04, 50ms 2:33, 60ms 2:41, 70ms 2:54, 80ms 3:04
            }
        }
        result.addWritten(nIndex);
        return result;
    }

    public void startProgressInfo(ProgressInfo progressInfo) {
        logger.info(CONSOLE_GUI_ONLY, progressInfo.getHeader());
    }

    public void updateProgressInfo(ProgressInfo progressInfo, long bytesDone) {
        if (progressInfo == null) {
            return;
        }

        progressInfo.update(bytesDone);
        printProgressInfo(progressInfo);
    }

    private void printProgressInfo(ProgressInfo progressInfo) {
        // append one space, just in case an exception message may come up
        String logText = String.format("%s%s%s%s ",
                AnsiCursor.off(),
                ansi().cursorToColumn(1).fgBright(GREEN).a(SpinningCursor.getNext()).reset(),
                progressInfo,
                AnsiCursor.on());
        logger.info(CONSOLE_GUI_NO_NEWLINE, logText);
    }

    public void logAndPrintProgressInfo(ProgressInfo progressInfo) {
        logger.debug("{}{}{}", progressInfo.getHeader(), System.lineSeparator(), progressInfo);
        SpinningCursor.setBlank();
        printProgressInfo(progressInfo);
        logger.info(CONSOLE_GUI_ONLY, ""); // Just a new line
    }

    public void programBootDescriptor(BootDescriptor bootDescriptor, int delay)
            throws UpdaterException, KNXLinkClosedException, InterruptedException, KNXTimeoutException {

        byte[] streamBootDescriptor = bootDescriptor.toStream();
        // send new boot descriptor
        ResponseResult flashResult = doFlash(streamBootDescriptor, getMaxUpdCommandRetry(), delay, null);
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
        logger.debug("Updating boot descriptor with crc32 {}",
                String.format("0x%08X, length %d", crc32Value, streamBootDescriptor.length));
        ResponseResult programResult = sendWithRetry(UPDCommand.UPDATE_BOOT_DESC, programBootDescriptor, getMaxUpdCommandRetry());
        if (UPDProtocol.checkResult(programResult.data()) != UDPResult.IAP_SUCCESS) {
            restartProgrammingDevice();
            throw new UpdaterException("Updating boot descriptor failed.");
        }
        programResult.addCounters(flashResult);
        if (delay > 0) {
            Thread.sleep(delay);
        }
    }

    public BootloaderStatistic requestBootLoaderStatistic() throws UpdaterException,
            InterruptedException {
        logger.debug("Requesting Bootloader statistic");
        byte[] result = sendWithRetry(UPDCommand.REQUEST_STATISTIC, new byte[0], getMaxUpdCommandRetry()).data();
        UPDCommand command = UPDCommand.tryFromByteArray(result);
        if (command != UPDCommand.RESPONSE_STATISTIC) {
            logger.warn("Requesting Bootloader statistic {}failed!{}", ansi().fg(RED), ansi().reset());
            return null;
        }
        BootloaderStatistic blStatistic = BootloaderStatistic.fromArray(Arrays.copyOfRange(result, DATA_POSITION, result.length));
        logger.debug("#Disconnect: {} #repeated T_ACK: {}", blStatistic.getDisconnectCountColored(),
                blStatistic.getRepeatedT_ACKcount());
        return blStatistic;
    }

    private boolean isLinkAlive() {
        if (link == null) {
            return false;
        }
        return link.isOpen();
    }

    private void handleKNXException(final UPDCommand command, final KNXException e, final boolean reconnect) throws
            UpdaterException, InterruptedException {
        if (e instanceof KNXAckTimeoutException) {
            // This exception is on missing/faulty ack at linklayer level thrown, but we never see it here
            // todo check public void send(final CEMI frame, final BlockingMode mode) in ConnectionBase
            logger.warn("{}Unexpected: never seen before {}{}", ansi().fg(RED), e.getClass().getSimpleName(),ansi().reset());
        }

        logger.warn("{}{} {} : {}{}", ansi().fg(RED), command, e.getMessage(),
                e.getClass().getSimpleName(), ansi().reset());

        if (!reconnect) {
            return;
        }

        try {
            reconnect(cliOptions.getReconnectMs());
        }
        catch (KNXException | UnknownHostException e2) {
            throw new UpdaterException(String.format("%s failed.", command), e);
        }
    }

    public ResponseResult sendWithRetry(final UPDCommand command, final byte[] data, int maxRetry)
            throws UpdaterException, InterruptedException {
        ResponseResult result = new ResponseResult();
        while (true) {
            KNXException lastCaughtException;
            try {
                byte[] data2 = mc.sendUpdateData(progDestination, command.toByte(), data);
                result.copyFromArray(data2);
                return result;
            }
            ///\todo check causes of KNXRemoteException, i think they are unrecoverable
            catch (KNXDisconnectException | KNXLinkClosedException e) {
                lastCaughtException = e;
                result.incDropCount();
                handleKNXException(command, e, true);
            }
            catch (KNXTimeoutException e) {
                // Can happen on a L2 tunnel request ACK timeout or a TL4 ACK timeout
                lastCaughtException = e;
                result.incTimeoutCount();
                handleKNXException(command, e, true);
            }
            catch (KNXInvalidResponseException e) {
                lastCaughtException = e;
                handleKNXException(command, e, true);
            }
            finally {
                if (!isLinkAlive()) {
                    maxRetry = 0; // exit while
                }
            }

            if (maxRetry > 0) {
                maxRetry--;
            }
            else {
                throw new UpdaterException(String.format("%s failed.", command), lastCaughtException);
            }
        }
    }

    public void checkDeviceInProgrammingMode(IndividualAddress progDeviceAddr) throws UpdaterException,
            InterruptedException {
        try {
            ManagementProcedures mgmt = new ManagementProceduresImpl(link);
            IndividualAddress[] devices = mgmt.readAddress();
            mgmt.close();
            if ((devices.length == 0) && (progDeviceAddr == null)) { // no device in prog mode
            	return;
            }
            else if ((devices.length == 1) && (progDeviceAddr != null) && (progDeviceAddr.equals(devices[0]))) { // correct device in prog mode
            	return;
            }
            logger.warn("{}{} Device(s) in bootloader/programming mode: {}{}",
                    ansi().fgBright(RED), devices.length, Arrays.stream(devices).toArray(), ansi().reset());
            if (devices.length == 0) {
                throw new UpdaterException("No device in programming mode.");
            }
            else {
                throw new UpdaterException(String.format("%d wrong device(s) %s are already in programming mode.", devices.length, Arrays.toString(devices)));
            }
        } catch (KNXException e ) {
            throw new UpdaterException(String.format("checkDevicesInProgrammingMode failed. %s", e.getMessage()), e);
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
        if (getProtocolVersion() == UDPProtocolVersion.UDP_V0) {
            return false;
        }
        this.blockSize = blockSize;
        return true;
    }

    public int getMaxPayload() {
        return maxPayload;
    }

    public int getMaxUpdCommandRetry() {
        return MAX_UPD_COMMAND_RETRY;
    }

    public String getLinkInfo() {
        if (this.link == null) {
            return "No link available.";
        }

        return link.toString();
    }
}
