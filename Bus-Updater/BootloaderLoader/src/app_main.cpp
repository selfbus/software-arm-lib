#include <sblib/internal/iap.h>
#include <sblib/io_pin_names.h>
#include <sblib/digital_pin.h>
#include <sblib/version.h>
#include <cstring>

#ifdef DEBUG
#   include <sblib/serial.h>
#endif

#ifdef DEBUG
#   define d(x) {x;}
#else
#   define d(x)
#endif

extern const __attribute__((aligned(16))) uint8_t incbin_bl_start[];
extern const uint8_t incbin_bl_end[];

#define BOOTLOADER_FLASH_STARTADDRESS ((uint8_t *) 0x0) //!< Flash start address of the bootloader
constexpr uint8_t LOADERLOADER_MAJOR_VERSION = 1;       //!< Bootloader Loader major version @note change also in @ref APP_VERSION
constexpr uint8_t LOADERLOADER_MINOR_VERSION = 0;       //!< Bootloader Loader minor Version @note change also in @ref APP_VERSION

APP_VERSION("SBloader", "1", "00");

void setup()
{
    pinMode(PIN_PROG, OUTPUT);
    digitalWrite(PIN_PROG, false);

    d(
        //serial.setRxPin(PIO3_1);
        //serial.setTxPin(PIO3_0);
        if (!serial.enabled())
        {
            serial.begin(115200);
        }
        serial.println("=========================================================");
        serial.print("Selfbus BootloaderLoader v", LOADERLOADER_MAJOR_VERSION);
        serial.print(".", LOADERLOADER_MINOR_VERSION, DEC, 3);
        serial.println(" DEBUG MODE :-)");
        serial.print("Build: ");
        serial.print(__DATE__);
        serial.print(" ");
        serial.println(__TIME__);
        serial.println("---------------------------------------------------------");
        serial.flush();
    );
}

int main()
{
    setup();

    const unsigned int newBlSize = (incbin_bl_end - incbin_bl_start);
    const unsigned int newBlStartSector = iapSectorOfAddress(BOOTLOADER_FLASH_STARTADDRESS);
    const unsigned int newBlEndSector = iapSectorOfAddress(BOOTLOADER_FLASH_STARTADDRESS + newBlSize - 1);
    d(
        serial.println("newBlSize: 0x", newBlSize, HEX, 4);
        serial.print("Erasing Sectors: ", newBlStartSector);
        serial.println(" - ", newBlEndSector);
    )

    iapEraseSectorRange(newBlStartSector, newBlEndSector);

    d(
        serial.println(" --> done");
    )

	for (const uint8_t * i = incbin_bl_start; i < incbin_bl_end; i += FLASH_SECTOR_SIZE)
	{
	    __attribute__ ((aligned (FLASH_RAM_BUFFER_ALIGNMENT))) byte buf[FLASH_SECTOR_SIZE]; // Address of buf must be word aligned, see iapProgram(..) hint.
		memset(buf, 0xFF, FLASH_SECTOR_SIZE);
		unsigned int len = incbin_bl_end - i;
		if (len > FLASH_SECTOR_SIZE)
		{
			len = FLASH_SECTOR_SIZE;
		}
		memcpy(buf, i, len);

		uint8_t * flash = BOOTLOADER_FLASH_STARTADDRESS + (i - incbin_bl_start);

		if (flash == nullptr)
		{
	        // NXP bootloader uses an Int-Vect as a checksum to see if the application is valid.
	        // If the value is not correct then it does not start the application
		    // Vector table start always at base address, each entry is 4 byte
		    uint32_t checksum = 0;
			for (int j = 0; j < 7; j++) // Checksum is 2's complement of entries 0 through 6
			{
				checksum += *(int*)&buf[j*4];
			}
			checksum = -checksum;
			*(int*)&buf[28] = checksum;
		    d(
		        serial.println("checksum: 0x", (int) checksum, HEX, 4);
		    )
		}
	    d(
	        serial.print("flashing 0x", flash);
	    )
		iapProgram(flash, buf, FLASH_SECTOR_SIZE);
	    d(
	        serial.println(" --> done");
	    )
	    digitalWrite(PIN_PROG, !digitalRead(PIN_PROG));
	}
    d(
        serial.println("RESET");
        serial.flush();
    )
	NVIC_SystemReset();
}
