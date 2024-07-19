Selfbus Bootloader and Updater
==============================

Bootloader and Updater for updating the application software of Selfbus devices.

Always use the bootloader and the Updater with the same version number!
*[Wiki entry about the Bootloader and Updater](https://selfbus.org/wiki/software/tools/7-selfbus-bus-updater-tool)*

## Important:
The memory settings of the application to be flashed must be set in MCUxpresso as follows:
*MCUxpresso under Project->Properties->C/C++ Build->MCU settings*:

When using the release version of the bootloader:

| Type  | Name     | Alias | Location   | Size   |
|:------|:---------|:------|:-----------|--------|
| Flash | MFlash64 | Flash | 0x3000     | 0xD000 |
| RAM   | RamLoc8  | RAM   | 0x10000100 | 0x1f00 |

When using the debug version of the bootloader:

| Type  | Name     | Alias | Location   | Size   |
|:------|:---------|:------|:-----------|--------|
| Flash | MFlash64 | Flash | 0x7000     | 0x9000 |
| RAM   | RamLoc8  | RAM   | 0x10000100 | 0x1f00 |
These memory settings can also be set automatically with this [linker-script](../examples/example-linkerscripts/memory.ldt).

## Build:
All build configurations link against the *release version* of the sblib.
- "Debug with UART (LPC11XX)" is set to compile with -Od (Optimize for debug) to fit into the first 0x7000 of flash
- "Debug (LPC11XX)" is also set to compile with -Od
- "Release (LPC11xx)" is set to compile with -Os (Optimize for size) to fit into the first 0x3000 of flash


# Deutsch:
Bootloader und Updater zum Updaten der Applikationssoftware von Selfbus Geräten

Immer den Bootloader und das PC-Tool mit der gleichen Versionsnummer verwenden!
*[Wiki-Eintrag zum Bootloader und Updater](https://selfbus.org/wiki/software/tools/7-selfbus-bus-updater-tool)*

## WICHTIG:
Die Speicher Einstellungen der zu flashenden App müssen in
*MCUxpresso unter Project->Properties->C/C++ Build->MCU settings* wie folgt eingestellt sein:

Bei Verwendung der Release Version des Bootloaders:

| Type  | Name     | Alias | Location   | Size   |
|:------|:---------|:------|:-----------|--------|
| Flash | MFlash64 | Flash | 0x3000     | 0xD000 |
| RAM   | RamLoc8  | RAM   | 0x10000100 | 0x1f00 |

Bei Verwendung der Debug Version des Bootloaders:

| Type  | Name     | Alias | Location   | Size   |
|:------|:---------|:------|:-----------|--------|
| Flash | MFlash64 | Flash | 0x7000     | 0x9000 |
| RAM   | RamLoc8  | RAM   | 0x10000100 | 0x1f00 |

Diese Speichereinstellung können auch automatisch durch dieses [Linkerscript](../examples/example-linkerscripts/memory.ldt) gesetzt werden.