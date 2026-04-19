set(SBLIB_LIB_TEST_CASES_RP2350_SRC
        # Common headers used by some tests
        src/tc_tlayer4_stepfunction.h
        src/tc_tlayer4_telegram.h

        # Entry point
        src/main.cpp

        # Portable tests (no protocol dependency)
        src/test_datapoint_types.cpp
        src/test_knx_lpdu.cpp

        # Protocol tests (use executeTest via MockKnxBus)
        src/prot_device_info.cpp
        src/prot_group_addresses.cpp
        src/prot_network_layer.cpp
        src/prot_parameter.cpp
        src/prot_physical_address.cpp
        src/test_prot_apci.cpp
        src/test_prot_app_program.cpp
        src/test_prot_tlayer4.cpp
        src/timeout_test.cpp
)
