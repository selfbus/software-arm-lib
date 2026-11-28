This directory contains the tests for the library.
The tests are executed on the PC, not on the ARM.

Building and running with CMake (Linux)
---------------------------------------
Requirements (Debian/Ubuntu): cmake (>= 3.28), ninja-build, g++.
For the x86 (32 bit) presets additionally: g++-multilib.

    cd test/lib-test-cases
    cmake --preset x64
    cmake --build --preset debug-x64
    ./cmake-build/x64/Debug/lib-test-cases_x64

Run only a group of tests by tag, e.g. all user EEPROM tests:

    ./cmake-build/x64/Debug/lib-test-cases_x64 "[userEeprom]"

build-all.sh builds all presets (x86, x64, with and without logging).
On Windows use build-all.ps1.

Building and running with MCUXpresso
------------------------------------
The tests can also be started from within MCUXpresso/Eclipse.

For a developer introduction to the test framework Catch, please see
https://github.com/philsquared/Catch/blob/master/docs/tutorial.md

To get colored output in the Eclipse console, install the Eclipse
plugin "Local Terminal". Linux only. For details see
http://stackoverflow.com/questions/6286701/an-eclipse-console-view-that-respects-ansi-color-codes
