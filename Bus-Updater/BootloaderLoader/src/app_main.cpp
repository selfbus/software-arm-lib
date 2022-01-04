#include <sblib/internal/iap.h>
#include <cstring>

extern const __attribute__((aligned(16))) unsigned int incbin_bl_start;
extern const unsigned int incbin_bl_end;
extern const unsigned int _image_start;

int main()
{
	int max = (int)&_image_start;
	max--;
	max /= 0x100;
	iapErasePageRange(0,max);
	for (unsigned int i = (unsigned int)&incbin_bl_start; i < (unsigned int)&incbin_bl_end; i += 0x1000)
	{
		byte buf[0x1000];
		memset(buf, 0xFF, 0x1000);
		unsigned int len = (unsigned int)&incbin_bl_end - i;
		if (len > 0x1000)
		{
			len = 0x1000;
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

		iapProgram((byte*)flash, buf, 0x1000);
	}
	NVIC_SystemReset();
}
