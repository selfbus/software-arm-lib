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

extern const __attribute__((aligned(16))) unsigned int incbin_bl_start;
extern const unsigned int incbin_bl_end;
extern const unsigned int _image_start;

#define BOOTLOADER_FLASH_STARTADDRESS ((unsigned int) 0x0) //!< Flash start address of the bootloader
#define LOADERLOADER_VERSION          (0x0001)             //!< boot loader loader Version 0.01

APP_VERSION("SBloader", "0", "01");

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
        serial.print("Selfbus BootloaderLoader V", LOADERLOADER_VERSION, HEX, 4);
        serial.println(", DEBUG MODE :-)");
        serial.print("Build: ");
        serial.print(__DATE__);
        serial.print(" ");
        serial.println(__TIME__);
        serial.println("---------------------------------------------------------");
    );
}

int main()
{
    setup();

    const unsigned int newBlSize = ((unsigned int) &incbin_bl_end - (unsigned int) &incbin_bl_start) + 1;
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

	for (unsigned int i = (unsigned int)&incbin_bl_start; i < (unsigned int)&incbin_bl_end; i += FLASH_SECTOR_SIZE)
	{
	    __attribute__ ((aligned (4))) byte buf[FLASH_SECTOR_SIZE]; // Address of buf must be word aligned, see iapProgram(..) hint.
		memset(buf, 0xFF, FLASH_SECTOR_SIZE);
		unsigned int len = (unsigned int)&incbin_bl_end - i;
		if (len > FLASH_SECTOR_SIZE)
		{
			len = FLASH_SECTOR_SIZE;
		}
		memcpy(buf, (void*)i, len);

		unsigned int flash = i - (unsigned int)&incbin_bl_start;

		if (flash == 0)
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
	        serial.print("flashing 0x", flash, HEX, 4);
	    )
		iapProgram((byte*)flash, buf, FLASH_SECTOR_SIZE);
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
