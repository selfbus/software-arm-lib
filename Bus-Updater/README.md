Bootloader und PC Tool
======================

Bootloader und PC Tool zum Updaten der Applikationssoftware von Selfbus Geräten

Immer den Bootloader und das PC-Tool mit der gleichen Versionsnummer verwenden!
*[Wiki-Eintrag zum Bus Updater Tool](https://selfbus.org/wiki/software/tools/7-selfbus-bus-updater-tool)*

## WICHTIG:
Die Speicher Einstellungen der zu flashenden App müssen in
*MCUxpresso unter Project->Properties->C/C++ Build->MCU settings* wie folgt eingestellt sein:

Bei Verwendung der Release Version des Bootloaders:
|Type   |Name    |Alias     |Location      |Size     |
|:------|:-------|:---------|:-------------|---------|
|Flash  |MFlash64|Flash     |0x3000        |0xD000   |
|RAM    |RamLoc8 |RAM       |0x10000100    |0x1f00   |

Bei Verwendung der Debug Version des Bootloaders:
|Type   |Name    |Alias     |Location      |Size     |
|:------|:-------|:---------|:-------------|---------|
|Flash  |MFlash64|Flash     |0x7000        |0x9000   |
|RAM    |RamLoc8 |RAM       |0x10000100    |0x1f00   |

Diese Speichereinstellung können auch automatisch durch dieses *[Linkerscript](https://github.com/selfbus/software-arm-lib/tree/master/examples/example-linkerscripts/memory.ldt)* gesetzt werden.