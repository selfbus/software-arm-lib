#ifndef IAQ_CORE_H
#define IAQ_CORE_H

class IAQcoreClass{
private:
	typedef enum {
	    IAQcoreAddress = 0xB4,
	} HUM_SENSOR_T;
public:
	bool Init(void);
	bool GetIAQ(uint16_t &CO2eq, uint8_t &status, uint32_t &resistance, uint16_t &Tvoc);
	bool GetIAQco2(uint16_t &CO2eq);
};

#endif
