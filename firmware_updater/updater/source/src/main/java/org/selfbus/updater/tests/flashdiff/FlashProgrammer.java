package org.selfbus.updater.tests.flashdiff;

import org.selfbus.updater.UpdaterException;
import io.calimero.KNXRemoteException;
import io.calimero.KNXTimeoutException;
import io.calimero.link.KNXLinkClosedException;
import io.calimero.mgmt.KNXDisconnectException;

import java.util.List;

public interface FlashProgrammer {

    void sendCompressedPage(List<Byte> outputDiffStream, long crc32) throws InterruptedException, KNXTimeoutException, KNXLinkClosedException, KNXRemoteException, KNXDisconnectException, UpdaterException;
}
