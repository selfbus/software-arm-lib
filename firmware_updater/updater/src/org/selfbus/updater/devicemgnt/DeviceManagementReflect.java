package org.selfbus.updater.devicemgnt;

import org.selfbus.updater.*;
import org.selfbus.updater.upd.UPDCommand;
import tuwien.auto.calimero.KNXException;

import java.net.UnknownHostException;

public class DeviceManagementReflect extends DeviceManagement {
    public DeviceManagementReflect(CliOptions cliOptions) {
        super(cliOptions);
    }

    @Override
    public ResponseResult sendWithRetry(final UPDCommand command, final byte[] data, int maxRetry)
            throws UpdaterException, InterruptedException {
        ReflectKNXnetIPTunnel reflect = new ReflectKNXnetIPTunnel(link);
        final int seqNmb =  reflect.getSequenceNumberSent();

        if ((cliOptions.isValidReconnectSeqNumber()) && (seqNmb >= cliOptions.getReconnectSeqNumber())) {
            try {
                this.reconnect(cliOptions.getReconnectMs());
            }
                catch (KNXException | UnknownHostException ignore) {
            }
        }
        return super.sendWithRetry(command, data, maxRetry);
    }
}
