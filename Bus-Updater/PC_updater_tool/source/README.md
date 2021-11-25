# Selfbus-updater 0.57

## Requirements

* JDK 11+
* gradle >=7.2

## Build
```
gradle fatJar
```
*or*
```
linux: gradlew fatJar
windows: gradlew.bat fatJar
```
*SB_updater-x.xx-all.jar* file is created in build/libs directory.

## Usage
```
SB_updater-0.57-all.jar <KNX Interface> [-f <flashFilename>] [-m <medium> | -s <serial>]  [-d
       <device>] [-D <progDevice>] [-u <uid>] [--full] [-H <localhost>] [-P <localport>] [-p <port>]
       [-n] [-r] [-a <appVersionPtr>] [-h | --version]  [--delay <delay>] [-v] [-f0]

Selfbus KNX-Firmware update tool options::
 -f,--fileName <flashFilename>        Filename of hex file to program
 -m,--medium <medium>                 KNX medium [tp1|rf] (default tp1)
 -s,--serial <serial>                 use FT1.2 serial communication
 -d,--device <device>                 KNX device address in normal operating mode (default none)
 -D,--progDevice <progDevice>         KNX device address in bootloader mode (default 15.15.192)
 -u,--uid <uid>                       send UID to unlock (default: request UID to unlock). Only the
                                      first 12 bytes of UID are used
    --full                            force full upload mode (disables differential mode)
 -H,--localhost <localhost>           local IP/host name
 -P,--localport <localport>           local UDP port (default system assigned)
 -p,--port <port>                     UDP port on <KNX Interface> (default 3671)
 -n,--nat                             enable Network Address Translation (NAT)
 -r,--routing                         use KNXnet/IP routing (not implemented)
 -a,--appVersionPtr <appVersionPtr>   pointer address to APP_VERSION string in new firmware file
 -h,--help                            show this help message
    --version                         show tool/library version
    --delay <delay>                   delay telegrams during data transmission to reduce bus load,
                                      valid 0-500ms, default 0
 -v,--verbose                         enable verbose status output (not used/implemented)
 -f0,--NO_FLASH                       for debugging use only, disable flashing firmware!
```

##Common use cases:
Recommended for new firmware versions if UID is unknown (requires active Prog. mode to unlock device):
```
java -jar SB_updater-x.xx-all.jar <ip address of KNX/IP GW> -fileName "out8-bcu1.hex" -nat
```
Recommended for new firmware versions with known UID:
```
java -jar SB_updater-x.xx-all.jar <ip address of KNX/IP GW> -fileName "out8-bcu1.hex" -uid 05:B0:01:02:E9:80:AC:AE:E9:07:47:55 -nat 
```
Manual specification of parameters if the App-Version pointer is not found/integrated in the firmware file:
```
java -jar SB_updater-x.xx-all.jar <ip address of KNX/IP GW> -fileName "in16-bim112.hex" -appVersionPtr 0x3263 -uid 05:B0:01:02:E9:80:AC:AE:E9:07:47:55 -nat 
```

##Used IDE's:
IntelliJ IDEA Community 2021.2.3 (Build -> Build Artifacts)<br>
eclipse project is currently not maintained

##gradle:
update [gradle wrapper](gradle/wrapper) to the newest version:
```
gradlew wrapper
```

list of gradle tasks:
```
gradlew tasks
```