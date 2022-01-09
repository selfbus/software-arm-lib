#include <sblib/internal/iap.h>
#include <sblib/io_pin_names.h>
#include <sblib/digital_pin.h>
#include <cstring>

extern const __attribute__((aligned(16))) unsigned int incbin_bl_start;
extern const unsigned int incbin_bl_end;
extern const unsigned int _image_start;

volatile const char __attribute__((used)) APP_VERSION[20] = "!AVP!@:SBloader0.10"; //!< bus updater magic string starting !AVP!@:

/**
 * @brief Helper function to always include @ref APP_VERSION in the resulting binary
 *        disable optimization seems to be the only way to ensure that this is not being removed by the linker
 *        to keep the variable, we need to declare a function that uses it.
 *        Alternatively the link script may be modified by adding KEEP to the section
 *
 * @return @ref APP_VERSION
 */
volatile const char * __attribute__((optimize("O0"))) getAppVersion()
{
    return APP_VERSION;
}
__attribute__((used))volatile const char * v = getAppVersion(); //!< just to ensure that APP_VERSION is not removed by the linker


void setup()
{
    pinMode(PIN_PROG, OUTPUT);
    digitalWrite(PIN_PROG, false);
}

int main()
{
    setup();
	int max = (int)&_image_start;
	max--;
	max /= FLASH_PAGE_SIZE;
	iapErasePageRange(0,max);
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
			uint32_t checksum = 0;
			for (int j = 0; j < 7; j++)
			{
				checksum += *(int*)&buf[j*4];
			}
			checksum = -checksum;
			*(int*)&buf[28] = checksum;
		}

		iapProgram((byte*)flash, buf, FLASH_SECTOR_SIZE);
		digitalWrite(PIN_PROG, !digitalRead(PIN_PROG));
	}
	NVIC_SystemReset();
}
