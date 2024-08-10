package org.selfbus.updater.mode.differential;

import org.selfbus.updater.UpdaterException;
import tuwien.auto.calimero.KNXTimeoutException;
import tuwien.auto.calimero.link.KNXLinkClosedException;

import java.util.List;

public interface FlashProgrammer {

    void sendCompressedPage(List<Byte> outputDiffStream, long crc32) throws InterruptedException, KNXTimeoutException, KNXLinkClosedException, UpdaterException;
}
