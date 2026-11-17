/*
 *  test_print.cpp - Unit tests for Print class
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include <sblib/print.h>
#include <vector>
#include <cstring>
#include <limits>
#include <climits> // INT_MIN

#include <catch.hpp> ///\todo replace with catch2/catch_test_macros.hpp


/**
 * Mock implementation of Print class for testing
 * Captures all written bytes into a buffer for verification
 */
class MockPrint final : public Print
{
public:
    std::vector<uint8_t> buffer;

    uint32_t write(const uint8_t ch) override
    {
        buffer.push_back(ch);
        return 1;
    }

    using Print::write;

    void clear()
    {
        buffer.clear();
    }

    [[nodiscard]] std::string getString() const {
        return {buffer.begin(), buffer.end()};
    }

    [[nodiscard]] size_t size() const
    {
        return buffer.size();
    }
};

struct PrintFloatTestCase {
    float value;
    const char* expected;
    uint8_t precision;
};

TEST_CASE("Print::", "[print]") {
    SECTION("print(char)")
    {
        MockPrint mock;

        SECTION("Print single character")
        {
            const uint32_t result = mock.print('A');
            REQUIRE(result == 1);
            REQUIRE(mock.getString() == "A");
        }

        SECTION("Print multiple characters")
        {
            mock.print('H');
            mock.print('i');
            REQUIRE(mock.getString() == "Hi");
        }
    }

    SECTION("print(const char*)")
    {
        MockPrint mock;

        SECTION("Print empty string")
        {
            uint32_t result = mock.print("");
            REQUIRE(result == 0);
            REQUIRE(mock.getString().empty());
        }

        SECTION("Print simple string")
        {
            uint32_t result = mock.print("Hello");
            REQUIRE(result == 5);
            REQUIRE(mock.getString() == "Hello");
        }

        SECTION("Print string with spaces")
        {
            uint32_t result = mock.print("Hello World");
            REQUIRE(result == 11);
            REQUIRE(mock.getString() == "Hello World");
        }

        SECTION("Print nullptr")
        {
            ///\todo error message:
            // more than one instance of overloaded function "MockPrint::print" matches the argument list:C/C++(308)
            // function "Print::print(const char *str)"
            // function "Print::print(const void *ptr)"

            // uint32_t result = mock.print(nullptr);
            // REQUIRE(result == 0);
            // REQUIRE(mock.getString() == "");
        }
    }

    SECTION("print<T> - signed integers")
    {
        MockPrint mock;

        SECTION("Print zero")
        {
            mock.print(static_cast<int8_t>(0));
            REQUIRE(mock.getString() == "0");
        }

        SECTION("Print positive number")
        {
            mock.print(static_cast<int16_t>(123));
            REQUIRE(mock.getString() == "123");
        }

        SECTION("Print negative number")
        {
            mock.print(-456);
            REQUIRE(mock.getString() == "-456");
        }

        SECTION("Print with minimum digits padding")
        {
            mock.print(42, DEC, 0);
            REQUIRE(mock.getString() == "42");
        }

        SECTION("Print negative with minimum digits")
        {
            mock.print(static_cast<int16_t>(-42), DEC, 0);
            REQUIRE(mock.getString() == "-42");
        }

        SECTION("Print with more digits padding")
        {
            mock.print(1234542, DEC, 20);
            REQUIRE(mock.getString() == "00000000000001234542");
        }

        SECTION("Print negative with more digits padding")
        {
            mock.print(static_cast<int16_t>(-42), DEC, 12);
            REQUIRE(mock.getString() == "-00000000042");
        }

        SECTION("Print minimum negative")
        {
            mock.print(INT_MIN, DEC);
            REQUIRE(mock.getString() == "-2147483648");
        }

        SECTION("Print minimum negative binary")
        {
            mock.print(INT_MIN, BIN);
            REQUIRE(mock.getString() == "-10000000000000000000000000000000");
        }
    }

    SECTION("print<T> - unsigned integers")
    {
        MockPrint mock;

        SECTION("Print decimal")
        {
            mock.print(static_cast<uint32_t>(12345), DEC);
            REQUIRE(mock.getString() == "12345");
        }

        SECTION("Print hexadecimal")
        {
            mock.print(static_cast<uint32_t>(255), HEX);
            REQUIRE(mock.getString() == "FF");
        }

        SECTION("Print hexadecimal with padding")
        {
            mock.print(static_cast<uint16_t>(15), HEX, 4);
            REQUIRE(mock.getString() == "000F");
        }

        SECTION("Print octal")
        {
            mock.print(static_cast<uint8_t>(64), OCT);
            REQUIRE(mock.getString() == "100");
        }

        SECTION("Print binary")
        {
            mock.print(static_cast<uint8_t>(5), BIN);
            REQUIRE(mock.getString() == "101");
        }

        SECTION("Print binary with padding")
        {
            mock.print(static_cast<uint16_t>(5), BIN, 8);
            REQUIRE(mock.getString() == "00000101");
        }

        SECTION("Print zero in different bases")
        {
            mock.print(static_cast<uint32_t>(0), DEC);
            REQUIRE(mock.getString() == "0");
            mock.clear();

            mock.print(static_cast<uint16_t>(0), HEX);
            REQUIRE(mock.getString() == "0");
            mock.clear();

            mock.print(static_cast<uint8_t>(0), BIN);
            REQUIRE(mock.getString() == "0");
        }
    }

    SECTION("print<T> - template integer types")
    {
        MockPrint mock;

        SECTION("Print uint8_t")
        {
            mock.print(static_cast<uint8_t>(255));
            REQUIRE(mock.getString() == "255");
        }

        SECTION("Print int8_t positive")
        {
            mock.print(static_cast<int8_t>(127));
            REQUIRE(mock.getString() == "127");
        }

        SECTION("Print int8_t negative")
        {
            mock.print(static_cast<int8_t>(-128));
            REQUIRE(mock.getString() == "-128");
        }

        SECTION("Print uint16_t")
        {
            mock.print(static_cast<uint16_t>(65535));
            REQUIRE(mock.getString() == "65535");
        }

        SECTION("Print int16_t")
        {
            mock.print(static_cast<int16_t>(-32768));
            REQUIRE(mock.getString() == "-32768");
        }

        SECTION("Print uint32_t")
        {
            mock.print(static_cast<uint32_t>(4294967295));
            REQUIRE(mock.getString() == "4294967295");
        }

        SECTION("Print int32_t")
        {
            // ReSharper disable once CppRedundantCastExpression
            mock.print(static_cast<int32_t>(-2147483647));
            REQUIRE(mock.getString() == "-2147483647");
        }

        SECTION("Print uintmax_t")
        {
            mock.print(static_cast<uintmax_t>(4294967295));
            REQUIRE(mock.getString() == "4294967295");
        }

        SECTION("Print intmax_t")
        {
            mock.print(static_cast<intmax_t>(-2147483647));
            REQUIRE(mock.getString() == "-2147483647");
        }
    }

    SECTION("print(float)")
    {
        MockPrint mock;

        SECTION("Print zero")
        {
            mock.print(0.0f);
            REQUIRE(mock.getString() == "0.00");
        }

        SECTION("Print negative zero")
        {
            mock.print(-0.0f);
            REQUIRE(mock.getString() == "-0.00");
        }

        SECTION("Check precision sanitation ")
        {
            mock.print(0.0f, Print::PRINT_FLOAT_MAX_PRECISION + 1);
            REQUIRE(mock.getString() == "0.0000000");
        }

        SECTION("Print NaN")
        {
            mock.print(std::numeric_limits<float>::quiet_NaN());
            REQUIRE(mock.getString() == "NaN");
        }

        SECTION("Print infinity")
        {
            mock.print(std::numeric_limits<float>::infinity());
            REQUIRE(mock.getString() == "inf");
        }

        SECTION("Print negative infinity")
        {
            mock.print(-std::numeric_limits<float>::infinity());
            REQUIRE(mock.getString() == "-inf");
        }

        SECTION("Print positive float default precision")
        {
            mock.print(3.14f);
            REQUIRE(mock.getString() == "3.14");
        }

        SECTION("Print negative float")
        {
            mock.print(-2.5f);
            REQUIRE(mock.getString() == "-2.50");
        }

        SECTION("Print float with precision 0")
        {
            mock.print(3.14159265f, 0);
            REQUIRE(mock.getString() == "3");
        }

        SECTION("Print float with precision 1")
        {
            mock.print(3.14159265f, 1);
            REQUIRE(mock.getString() == "3.1");
        }

        SECTION("Print float with precision 2")
        {
            mock.print(3.14159265f, 2);
            REQUIRE(mock.getString() == "3.14");
        }

        SECTION("Print float with precision 3")
        {
            mock.print(3.14159265f, 3);
            REQUIRE(mock.getString() == "3.141");
        }

        SECTION("Print float with precision 4")
        {
            mock.print(3.14159265f, 4);
            REQUIRE(mock.getString() == "3.1415");
        }

        SECTION("Print float with precision 5")
        {
            mock.print(3.14159265f, 5);
            REQUIRE(mock.getString() == "3.14159");
        }

        SECTION("Print float with precision 6")
        {
            mock.print(3.14159265f, 6);
            REQUIRE(mock.getString() == "3.141592");
        }

        SECTION("Print float with maximum precision 7")
        {
            mock.print(3.14159265f, 7); // actual float is 3.14159274
            REQUIRE(mock.getString() == "3.1415927");
        }

        SECTION("Print negative float with maximum precision 7")
        {
            mock.print(-3.14159265f, 7); // actual float is 3.14159274
            REQUIRE(mock.getString() == "-3.1415927");
        }

        SECTION("Print zero float")
        {
            mock.print(0.0f);
            REQUIRE(mock.getString() == "0.00");
        }

        SECTION("Print integer as float")
        {
            mock.print(5.0f);
            REQUIRE(mock.getString() == "5.00");
        }

        SECTION("Print small positive float")
        {
            mock.print(0.0000001f, 7);
            REQUIRE(mock.getString() == "0.0000001");
        }

        SECTION("Print small positive float with default precision")
        {
            mock.print(0.0000001f);
            REQUIRE(mock.getString() == "0.00");
        }

        SECTION("Print small negative float")
        {
            mock.print(-0.0000004f, 7);
            REQUIRE(mock.getString() == "-0.0000004");
        }

        SECTION("Print large float")
        {
            mock.print(123456.789f, 3);
            REQUIRE(mock.getString() == "123456.789");
        }

        SECTION("Print large float with default precision")
        {
            mock.print(999999.5f);
            REQUIRE(mock.getString() == "999999.50");
        }

        SECTION("Print large negative float")
        {
            mock.print(-987654.312f, 3);
            REQUIRE(mock.getString() == "-987654.312");
        }

        SECTION("Print max float value without overflow")
        {
            mock.print(4.5E18f);
            REQUIRE(mock.getString() == "4500000101179064320.00");
        }

        SECTION("Print max float value")
        {
            float maxFloat = std::numeric_limits<float>::max();
            // maxFloat is here actually 3.40282347e+38
            mock.print(maxFloat, 2);
            REQUIRE(mock.getString() == "overflow");
        }

        SECTION("Print min float value")
        {
            float minFloat = std::numeric_limits<float>::min();
            mock.print(minFloat, 7);
            REQUIRE(mock.getString() == "0.0000000");
        }

        SECTION("Print scientific notation float")
        {
            mock.print(1.2E3f, 2);
            REQUIRE(mock.getString() == "1200.00");
            mock.clear();
            mock.print(1.5E-3f, 4);
            REQUIRE(mock.getString() == "0.0015");
            mock.clear();
            mock.print(3.14E2f, 1);
            REQUIRE(mock.getString() == "314.0");
            mock.clear();
            mock.print(2.5E-4f, 5);
            REQUIRE(mock.getString() == "0.00025");
            mock.clear();
            mock.print(1.0E6f);
            REQUIRE(mock.getString() == "1000000.00");
            mock.clear();
            mock.print(5.5E-6f, 7);
            REQUIRE(mock.getString() == "0.0000055");
            mock.clear();
            mock.print(-1.23E4f, 1);
            REQUIRE(mock.getString() == "-12300.0");
            mock.clear();
            mock.print(-7.89E-5f, 6);
            REQUIRE(mock.getString() == "-0.000078");
        }

        SECTION("Print supported scientific notation float")
        {
            constexpr auto maxPrecision = Print::PRINT_FLOAT_MAX_PRECISION;
            constexpr uint8_t noPrecision = 0;
            PrintFloatTestCase testCases[] = {
                // "Large" floats
                {1.2E0f, "1.2000000", maxPrecision},
                {1.23E1f, "12.3000001", maxPrecision},
                {1.234E2f, "123.4000015", maxPrecision},
                {1.2345E3f, "1234.5000000", maxPrecision},
                {1.23456E4f, "12345.5996093", maxPrecision},
                {1.234567E5f, "123456.7031250", maxPrecision},
                {1.2345678E6f, "1234567.7500000", maxPrecision},
                {1.23456789E7f, "12345679.0000000", maxPrecision},
                {1.234567891E8f, "123456792.0000000", maxPrecision},
                {2.2345678912E9f, "2234567936", noPrecision},
                {3.23456789123E10f, "32345679872", noPrecision},
                {4.234567891234E11f, "423456800768", noPrecision},
                {5.2345678912345E12f, "5234567938048", noPrecision},
                {6.23456789123456E13f, "62345678159872", noPrecision},
                {7.234567891234567E14f, "723456773586944", noPrecision},
                {8.2345678912345678E15f, "8234567924187136", noPrecision},
                {9.23456789123456789E16f, "92345679514435584", noPrecision},
                {1.234567891234567891E17f, "123456790519087104", noPrecision},
                {1.2345678912345678912E18f, "1234567939550609408", noPrecision},
                {1.23456789123456789123E19f, "12345679395506094080", noPrecision},
                {1.234567891234567891234E20f, "overflow", maxPrecision},

                // "Small" floats
                {0.12E0f, "0.1199999", maxPrecision},
                {0.123E-1f, "0.0122999", maxPrecision},
                {0.1234E-2f, "0.0012339", maxPrecision},
                {0.12345E-3f, "0.0001234", maxPrecision},
                {0.123456E-4f, "0.0000123", maxPrecision},
                {0.1234567E-5f, "0.0000012", maxPrecision},
                {0.12345678E-6f, "0.0000001", maxPrecision},
                {0.123456789E-7f, "0.0000000", maxPrecision},
                {0.1234567891E-8f, "0.0000000", maxPrecision},
                {0.1234567891E-38f, "0.0000000", maxPrecision},

                // Denormalized floats
                {1.40129846e-39f, "0.0000000", maxPrecision},
                {1.40129846e-45f, "0.0000000", maxPrecision},
            };
            for (const auto&[value, expected, precision] : testCases)
            {
                mock.print(value, precision);
                REQUIRE(mock.getString() == expected);
                mock.clear();
            }
        }

        SECTION("Print small positive/negative scientific notation float")
        {
            mock.print(1.2E-37f, 2);
            REQUIRE(mock.getString() == "0.00");
            mock.clear();
            mock.print(-1.2E-37f, 2);
            REQUIRE(mock.getString() == "-0.00");

            mock.clear();
            mock.print(1.5E-37f, 4);
            REQUIRE(mock.getString() == "0.0000");
            mock.clear();
            mock.print(-1.5E-37f, 4);
            REQUIRE(mock.getString() == "-0.0000");

            mock.clear();
            mock.print(3.14E-37f, 1);
            REQUIRE(mock.getString() == "0.0");
            mock.clear();
            mock.print(-3.14E-37f, 1);
            REQUIRE(mock.getString() == "-0.0");

            mock.clear();
            mock.print(2.5E-36f, 5);
            REQUIRE(mock.getString() == "0.00000");
            mock.clear();
            mock.print(-2.5E-36f, 5);
            REQUIRE(mock.getString() == "-0.00000");

            mock.clear();
            mock.print(2.5E-35f, 7);
            REQUIRE(mock.getString() == "0.0000000");
            mock.clear();
            mock.print(-2.5E-35f, 7);
            REQUIRE(mock.getString() == "-0.0000000");
        }
    }

    SECTION("print(const char*, value) - string + value combinations")
    {
        MockPrint mock;

        SECTION("String + signed integer")
        {
            mock.print("Value: ", static_cast<int8_t>(42));
            REQUIRE(mock.getString() == "Value: 42");
        }

        SECTION("String + unsigned integer")
        {
            mock.print("Hex: ", static_cast<uint16_t>(255), HEX);
            REQUIRE(mock.getString() == "Hex: FF");
        }

        SECTION("String + uint8_t")
        {
            mock.print("Count: ", static_cast<uint8_t>(100));
            REQUIRE(mock.getString() == "Count: 100");
        }

        SECTION("String + float")
        {
            mock.print("Pi: ", 3.14f);
            REQUIRE(mock.getString() == "Pi: 3.14");
        }
    }

    SECTION("print(void*) - pointer printing")
    {
        MockPrint mock;

        SECTION("Print pointer")
        {
            const auto ptr = reinterpret_cast<void*>(0x12345678);
            mock.print(ptr);
            // Pointer should be printed in hex with proper width
            std::string result = mock.getString();
            REQUIRE(result.find("12345678") != std::string::npos);
            REQUIRE(result.length() == 2 * sizeof(void*));
        }

        SECTION("Print nullptr")
        {
            const void* ptr = nullptr;
            mock.print(ptr);
            std::string result = mock.getString();
            REQUIRE(result.find("00000000") != std::string::npos);
            REQUIRE(result.length() == 2 * sizeof(void*));
#if INTPTR_MAX == INT64_MAX
            REQUIRE(result == "0000000000000000");
#endif
        }

#if INTPTR_MAX == INT64_MAX
        SECTION("Print x64 pointer")
        {
            const auto ptr = reinterpret_cast<void*>(0xcafedeadbeefaffe);
            mock.print(ptr);
            REQUIRE(mock.getString() == "CAFEDEADBEEFAFFE");
        }
#endif

        SECTION("String + pointer")
        {
            const auto ptr = reinterpret_cast<void*>(0xABCD);
            mock.print("Address: ", ptr);
            std::string result = mock.getString();
            REQUIRE(result.find("Address: ") == 0);
            REQUIRE(result.find("ABCD") != std::string::npos);
        }
    }

    SECTION("println() - newline variants")
    {
        MockPrint mock;

        SECTION("Print newline only")
        {
            uint32_t result = mock.println();
            REQUIRE(result == 2);
            REQUIRE(mock.getString() == "\r\n");
        }

        SECTION("Print string with newline")
        {
            mock.println("Hello");
            REQUIRE(mock.getString() == "Hello\r\n");
        }

        SECTION("Print integer with newline")
        {
            mock.println(static_cast<int8_t>(42));
            REQUIRE(mock.getString() == "42\r\n");
        }

        SECTION("Print unsigned with newline")
        {
            mock.println(static_cast<uint16_t>(100), HEX);
            REQUIRE(mock.getString() == "64\r\n");
        }

        SECTION("Print uint8_t with newline")
        {
            mock.println(static_cast<uint8_t>(255));
            REQUIRE(mock.getString() == "255\r\n");
        }

        SECTION("Print float with newline")
        {
            mock.println(2.5f);
            REQUIRE(mock.getString() == "2.50\r\n");
        }

        SECTION("Print string + value with newline")
        {
            mock.println("Result: ", 123);
            REQUIRE(mock.getString() == "Result: 123\r\n");
        }

        SECTION("Print string + unsigned with newline")
        {
            mock.println("Hex: ", static_cast<uint16_t>(255), HEX);
            REQUIRE(mock.getString() == "Hex: FF\r\n");
        }

        SECTION("Print string + uint16_t with newline")
        {
            mock.println("Value: ", static_cast<uint16_t>(1000));
            REQUIRE(mock.getString() == "Value: 1000\r\n");
        }

        SECTION("Print pointer with newline")
        {
            auto ptr = reinterpret_cast<void*>(0x1234);
            mock.println(ptr);
            std::string result = mock.getString();
            REQUIRE(result.find("1234") != std::string::npos);
            REQUIRE(result.find("\r\n") != std::string::npos);
        }

        SECTION("Print string + pointer with newline")
        {
            auto ptr = reinterpret_cast<void*>(0xABCD);
            mock.println("Ptr: ", ptr);
            std::string result = mock.getString();
            REQUIRE(result.find("Ptr: ") == 0);
            REQUIRE(result.find("ABCD") != std::string::npos);
            REQUIRE(result.find("\r\n") != std::string::npos);
        }

        SECTION("Print string + float with newline")
        {
            mock.println("Value: ", 1.5f, 1);
            REQUIRE(mock.getString() == "Value: 1.5\r\n");
        }
    }

    SECTION("write(const byte*, uint32_t)")
    {
        MockPrint mock;

        SECTION("Write byte array")
        {
            uint8_t data[] = {65, 66, 67}; // "ABC"
            uint32_t result = mock.write(&data[0], 3);
            REQUIRE(result == 3);
            REQUIRE(mock.getString() == "ABC");
        }

        SECTION("Write empty array")
        {
            uint8_t data[] = {65};
            uint32_t result = mock.write(data, 0);
            REQUIRE(result == 0);
            REQUIRE(mock.getString().empty());
        }

        SECTION("Write single byte via array")
        {
            uint8_t data[] = {88};
            mock.write(data, 1);
            REQUIRE(mock.getString() == "X");
        }
    }

    SECTION("write(const char*)")
    {
        MockPrint mock;

        SECTION("Write string")
        {
            uint32_t result = mock.write("Test");
            REQUIRE(result == 4);
            REQUIRE(mock.getString() == "Test");
        }

        SECTION("Write empty string")
        {
            uint32_t result = mock.write("");
            REQUIRE(result == 0);
            REQUIRE(mock.getString().empty());
        }

        SECTION("Write nullptr")
        {
            // ReSharper disable once CppRedundantCastExpression
            uint32_t result = mock.write(static_cast<const char*>(nullptr));
            REQUIRE(result == 0);
            REQUIRE(mock.getString().empty());
        }
    }

    SECTION("Formatting scenarios")
    {
        MockPrint mock;

        SECTION("Mixed print calls")
        {
            mock.print("Count: ");
            mock.print(10);
            mock.print(", Hex: ");
            mock.print(static_cast<uint16_t>(255), HEX);
            mock.println();
            REQUIRE(mock.getString() == "Count: 10, Hex: FF\r\n");
        }

        SECTION("Multiple lines")
        {
            mock.println("Line 1");
            mock.println("Line 2");
            mock.println("Line 3");
            REQUIRE(mock.getString() == "Line 1\r\nLine 2\r\nLine 3\r\n");
        }

        SECTION("Large number formatting")
        {
            mock.print(0xFFFFFFFF, HEX);
            REQUIRE(mock.getString() == "FFFFFFFF");
        }

        SECTION("Binary representation of byte")
        {
            mock.print(static_cast<uint8_t>(0b10101010), BIN, 8);
            REQUIRE(mock.getString() == "10101010");
        }
    }

    SECTION("Edge cases", "")
    {
        MockPrint mock;

        SECTION("Very small float")
        {
            mock.print(0.001f, 3);
            REQUIRE(mock.getString() == "0.001");
        }

        SECTION("Float rounding")
        {
            mock.print(1.995f, 2);
            // Result depends on rounding behavior
            std::string result = mock.getString();
            REQUIRE((result == "1.99" || result == "2.00"));
        }

        SECTION("Large negative number")
        {
            mock.print(-999999);
            REQUIRE(mock.getString() == "-999999");
        }

        SECTION("Zero padding with various bases")
        {
            mock.print(static_cast<uint16_t>(8), DEC, 3);
            REQUIRE(mock.getString() == "008");
            mock.clear();

            mock.print(static_cast<uint32_t>(8), HEX, 3);
            REQUIRE(mock.getString() == "008");
            mock.clear();

            mock.print(static_cast<uint8_t>(8), OCT, 3);
            REQUIRE(mock.getString() == "010");
            mock.clear();

            mock.print(static_cast<uint8_t>(8), BIN, 8);
            REQUIRE(mock.getString() == "00001000");
        }
    }

    SECTION("Return value verification")
    {
        SECTION("Return values match written bytes")
        {
            MockPrint mock;
            REQUIRE(mock.print("") == 0);
            mock.clear();

            REQUIRE(mock.print("Test") == 4);
            mock.clear();

            REQUIRE(mock.print(123) == 3);
            mock.clear();

            REQUIRE(mock.println("Hello") == 7); // 5 chars + \r\n
            mock.clear();

            REQUIRE(mock.print(255, HEX) == 2); // "FF"
        }
    }

    SECTION("Bugs found during development")
    {
        SECTION("call of overloaded 'print(long int, Base)' is ambiguous")
        {
            MockPrint mock;
            int32_t value = 42;
            mock.print(value / 100, DEC);
            REQUIRE(mock.getString() == "0");
        }
        SECTION("call of overloaded 'print(long int, Base, int)' is ambiguous")
        {
            MockPrint mock;
            int32_t value = 42;
            mock.print(std::abs(value % 100), DEC, 2);
            REQUIRE(mock.getString() == "42");
        }

        SECTION("call with enum")
        {
            enum TestEnum : uint8_t
            {
                VALUE1 = 1,
                VALUE2 = 2
            };
            MockPrint mock;
            TestEnum value = VALUE1;
            REQUIRE(mock.print(value));
        }
    }
}
