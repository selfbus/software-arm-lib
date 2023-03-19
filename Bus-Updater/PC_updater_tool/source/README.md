# Selfbus-Updater 0.91

## Requirements

* JDK 11+
* gradle >=7.4
* Selfbus device with flashed [bus-updater/bootloader](../) version 0.91 or higher

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
java -jar SB_updater-x.xx-all.jar <KNX Interface> [-f <filename>] [-m <tp1|rf> | -s <COM-port> | -t
       <COM-port>]   [-d <x.x.x>] [-D <x.x.x>] [-o <x.x.x>] [--priority <SYSTEM|NORMAL|URGENT|LOW>]
       [--user <id>] [--user-pwd <password>] [--device-pwd <password>] [-u <uid>] [-f1] [-H
       <localhost>] [-P <localport>] [-p <port>] [-n] [-r] [-a <address>] [-h | -v]  [--delay <ms>]
       [-l <TRACE|DEBUG|INFO>] [--ERASEFLASH] [--DUMPFLASH <start> <end>] [-f0]

Selfbus KNX-Firmware update tool options:
 -f,--fileName <filename>                   Filename of hex file to program
 -m,--medium <tp1|rf>                       KNX medium [tp1|rf] (default tp1)
 -s,--serial <COM-port>                     use FT1.2 serial communication
 -t,--tpuart <COM-port>                     use TPUART serial communication (experimental, needs
                                            serialcom or rxtx library in java.library.path
 -d,--device <x.x.x>                        KNX device address in normal operating mode (default
                                            none)
 -D,--progDevice <x.x.x>                    KNX device address in bootloader mode (default
                                            15.15.192)
 -o,--own <x.x.x>                           own physical KNX address (default 0.0.0)
    --priority <SYSTEM|NORMAL|URGENT|LOW>   KNX telegram priority (default LOW)
    --user <id>                             KNX IP Secure tunneling user identifier (1..127)
                                            (default 1)
    --user-pwd <password>                   KNX IP Secure tunneling user password (Commissioning
                                            password/Inbetriebnahmepasswort), " in password may not
                                            work
    --device-pwd <password>                 KNX IP Secure device authentication code (Authentication
                                            Code/Authentifizierungscode) " in password may not work
 -u,--uid <uid>                             send UID to unlock (default: request UID to unlock).
                                            Only the first 12 bytes of UID are used
 -f1,--full                                 force full upload mode (disables differential mode)
 -H,--localhost <localhost>                 local IP/host name
 -P,--localport <localport>                 local UDP port (default system assigned)
 -p,--port <port>                           UDP port on <KNX Interface> (default 3671)
 -n,--nat                                   enable Network Address Translation (NAT)
 -r,--routing                               use KNXnet/IP routing (not implemented)
 -a,--appVersionPtr <address>               pointer address to APP_VERSION string in new firmware
                                            file
 -h,--help                                  show this help message
 -v,--version                               show tool/library version
    --delay <ms>                            delay telegrams during data transmission to reduce bus
                                            load, valid 0-500ms, default 0
 -l,--logLevel <TRACE|DEBUG|INFO>           Logfile logging level [TRACE|DEBUG|INFO] (default DEBUG)
    --ERASEFLASH                            USE WITH CAUTION! Erases the complete flash memory
                                            including the physical KNX address and all settings of
                                            the device. Only the bootloader is not deleted.
    --DUMPFLASH <start> <end>               dump a flash range in intel(R) hex to the serial port of
                                            the MCU. Works only with DEBUG version of the
                                            bootloader.
 -f0,--NO_FLASH                             for debugging use only, disable flashing firmware!
    --statistic                             show more statistic data
```
## Common use cases:
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
## Used IDE's:
IntelliJ IDEA Community 2022.3.1 (Build -> Build Artifacts)<br>
eclipse project is currently not maintained
## gradle:
update [gradle wrapper](gradle/wrapper) to the newest version:
```
gradlew wrapper
```

list of gradle tasks:
```
gradlew tasks
```