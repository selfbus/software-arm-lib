package org.selfbus.updater.devicemgnt;

import org.selfbus.updater.CliOptions;

final public class DeviceManagementFactory {
    @SuppressWarnings("unused")
    private DeviceManagementFactory() {}

    public static DeviceManagement getDeviceManagement(CliOptions cliOptions) {
        if (cliOptions.isValidReconnectSeqNumber()) {
            return new DeviceManagementReflect(cliOptions);
        }
        else {
            return new DeviceManagement(cliOptions);
        }
    }
}
