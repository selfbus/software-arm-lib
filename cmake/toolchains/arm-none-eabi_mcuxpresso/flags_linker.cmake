# Create initial Linker flags
get_filename_component(CURRENT_FILE_NAME ${CMAKE_CURRENT_LIST_FILE} NAME)
message(STATUS "Linker flags in ${CURRENT_FILE_NAME} are untested.")
set(LINKER_FLAGS_LIST
    # TODO linker flags are untested
    -nostdlib -mcpu=cortex-m0 -mthumb
    -Xlinker --cref
    -Xlinker --gc-sections
    -Xlinker -print-memory-usage
    # Keep the handlers global with LTO. Otherwise the linker script does not see them and calculates the
    # boot ROM checksum (__valid_user_code_checksum) from the wrong addresses.
    -Xlinker --undefined=NMI_Handler
    -Xlinker --undefined=HardFault_Handler
)
string(JOIN " " LINKER_FLAGS ${LINKER_FLAGS_LIST})
set(LINKER_FLAGS_DEBUG "-DDEBUG")
set(LINKER_FLAGS_RELEASE "-DNDEBUG -flto -Os")