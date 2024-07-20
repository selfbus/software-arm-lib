Selfbus Updater, Bootloader and Bootloaderupdater
=================================================

Bootloader and Updater for updating the application software of Selfbus devices.

If possible, the Updater and Bootloader should be of the same version number.
*[Wiki entry about the Bootloader and Updater](https://selfbus.org/wiki/software/tools/7-selfbus-bus-updater-tool)*

## Important:
The memory settings of the application to be flashed must be set in MCUxpresso as follows:
*MCUxpresso under Project->Properties->C/C++ Build->MCU settings*:

When using the release version of the bootloader:

| Type  | Name     | Alias | Location   | Size   |
|:------|:---------|:------|:-----------|--------|
| Flash | MFlash64 | Flash | 0x3000     | 0xD000 |
| RAM   | RamLoc8  | RAM   | 0x100000c0 | 0x1f40 |

When using the debug version of the bootloader:

| Type  | Name     | Alias | Location   | Size   |
|:------|:---------|:------|:-----------|--------|
| Flash | MFlash64 | Flash | 0x7000     | 0x9000 |
| RAM   | RamLoc8  | RAM   | 0x100000c0 | 0x1f40 |
These memory settings can also be set automatically with this [linker-script](../examples/example-linkerscripts/memory.ldt).

## Build:
Most build configurations link against the *release version* of the sblib.
- "Debug low level" is set to compile with -Od (Optimize for debug) to fit into the first 0x7000 of flash
- "Debug" is also set to compile with -Od
- "Release" is set to compile with -Os (Optimize for size) to fit into the first 0x3000 of flash


# Deutsch:
Bootloader und Updater zum Updaten der Applikationssoftware von Selfbus Geräten

Wenn möglich sollten Updater und Bootloader mit derselben Versionsnummer verwendet werden.
*[Wiki-Eintrag zum Bootloader und Updater](https://selfbus.org/wiki/software/tools/7-selfbus-bus-updater-tool)*

## WICHTIG:
Die Speicher Einstellungen der zu flashenden App müssen in
*MCUxpresso unter Project->Properties->C/C++ Build->MCU settings* wie folgt eingestellt sein:

Bei Verwendung der Release Version des Bootloaders:

| Type  | Name     | Alias | Location   | Size   |
|:------|:---------|:------|:-----------|--------|
| Flash | MFlash64 | Flash | 0x3000     | 0xD000 |
| RAM   | RamLoc8  | RAM   | 0x100000c0 | 0x1f40 |

Bei Verwendung der Debug Version des Bootloaders:

| Type  | Name     | Alias | Location   | Size   |
|:------|:---------|:------|:-----------|--------|
| Flash | MFlash64 | Flash | 0x7000     | 0x9000 |
| RAM   | RamLoc8  | RAM   | 0x100000c0 | 0x1f40 |

Diese Speichereinstellung können auch automatisch durch dieses [Linkerscript](../examples/example-linkerscripts/memory.ldt) gesetzt werden.