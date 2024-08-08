# Selfbus-Updater 1.20

## Requirements

* JDK 17+
* gradle >=8.5
* Selfbus device with flashed [bootloader](../bootloader) version 1.00 or higher

## Build
```
gradle fatJar
```
*or*
```
linux: gradlew fatJar
windows: gradlew.bat fatJar
```
*SB_updater-x.xx-all.jar* file is created in [build/libs](source/build/libs) directory.

## Usage
```
java -jar SB_updater-x.xx-all.jar <KNX Interface> [-f <filename>] [-m <tp1|rf> | -s <COM-port> | -t
       <COM-port>]   [-d <x.x.x>] [-D <x.x.x>] [-o <x.x.x>] [--priority <SYSTEM|URGENT|NORMAL|LOW>]
       [-bs <256|512|1024>] [--user <id>] [--user-pwd <password>] [--device-pwd <password>] [-u
       <uid>] [-f1] [-H <localhost>] [-P <localport>] [-p <port>] [-t2] [-t1] [-n] [-r] [-h | -v]
       [--delay <ms>] [-l <TRACE|DEBUG|INFO>] [--ERASEFLASH] [--DUMPFLASH <start> <end>] [-f0]
       [--statistic]

Selfbus KNX-Firmware update tool options:
 -f,--fileName <filename>                   Filename of hex file to program
 -m,--medium <tp1|rf>                       KNX medium [tp1|rf] (default tp1)
 -s,--serial <COM-port>                     use FT1.2 serial communication
 -t,--tpuart <COM-port>                     use TPUART serial communication (experimental, needs
                                            serialcom or rxtx library in java.library.path)
 -d,--device <x.x.x>                        KNX device address in normal operating mode (default
                                            none)
 -D,--progDevice <x.x.x>                    KNX device address in bootloader mode (default
                                            15.15.192)
 -o,--own <x.x.x>                           own physical KNX address (default 0.0.0)
    --priority <SYSTEM|URGENT|NORMAL|LOW>   KNX telegram priority (default LOW)
 -bs,--blocksize <256|512|1024>             Block size to program (default 1024 bytes)
    --user <id>                             KNX IP Secure tunneling user identifier (1..127)
                                            (default 1)
    --user-pwd <password>                   KNX IP Secure tunneling user password (Commissioning
                                            password/Inbetriebnahmepasswort), quotation marks (") in
                                            password may not work
    --device-pwd <password>                 KNX IP Secure device authentication code (Authentication
                                            Code/Authentifizierungscode) quotation marks(") in
                                            password may not work
 -u,--uid <uid>                             send UID to unlock (default: request UID to unlock).
                                            Only the first 12 bytes of UID are used
 -f1,--full                                 force full upload mode (disables differential mode)
 -H,--localhost <localhost>                 local IP/host name
 -P,--localport <localport>                 local UDP port (default system assigned)
 -p,--port <port>                           UDP port on <KNX Interface> (default 3671)
 -t2,--tunnelingv2                          use KNXnet/IP tunneling v2 (TCP) (experimental)
 -t1,--tunneling                            use KNXnet/IP tunneling v1 (UDP)
 -n,--nat                                   enable Network Address Translation (NAT) (only available
                                            with tunneling v1)
 -r,--routing                               use KNXnet/IP routing/multicast (experimental)
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
Updater with graphical user interface (**experimental**)
```
java -jar SB_updater-x.xx-all.jar
```
Read UID of the device:
```
java -jar SB_updater-x.xx-all.jar <ip address of KNX/IP GW>
```
Recommended for new firmware versions if UID is unknown (requires active programming mode to unlock the device):
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
## Development

### IDEs:
- [IntelliJ IDEA (Community Edition)](https://www.jetbrains.com/idea/download)
- Eclipse project is currently not maintained

### IntelliJ IDEA Settings for Updater GUI development:
Change these in [**Settings Dialog**](https://www.jetbrains.com/help/idea/settings-preferences-dialog.html) (Menu File->Settings):
- Editor->[GUI Designer](https://www.jetbrains.com/help/idea/gui-designer.html)->Generate GUI into: Java source code
- [Plugins](https://www.jetbrains.com/help/idea/plugins-settings.html)->install "Resource Bundle Editor" [(howto)](https://www.jetbrains.com/help/idea/resource-bundle.html#open-bundle-editor)
- Build, Execution, Deployment->[Build Tools](https://www.jetbrains.com/help/idea/settings-build-tools.html)->Gradle->Build and run using: Intellij IDEA
- Build, Execution, Deployment->[Build Tools](https://www.jetbrains.com/help/idea/settings-build-tools.html)->Gradle->Run tests using: Intellij IDEA


### gradle:
update [gradle wrapper](source/gradle/wrapper) to the newest version:
```
gradlew wrapper
```

list of gradle tasks:
```
gradlew tasks
```