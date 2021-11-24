#ifndef sblib_usereeprom_bcu1_h
#define sblib_usereeprom_bcu1_h

#include <sblib/eib/userEeprom.h>

/**
 * The user EEPROM.
 *
 * The user EEPROM can be accessed by name, like userEeprom.status and as an array, like
 * userEeprom[addr]. Please note that the start address of the EEPROM is subtracted. That means
 * userEeprom[0x107] is the correct address for userEeprom.version; not userEeprom[0x07].
 */
class UserEepromBCU1 : public UserEeprom
{
public:
	UserEepromBCU1(Bus *bus) : UserEeprom(bus, 0x100, 156) {};
	virtual ~UserEepromBCU1() = default;

    virtual int userEepromFlashSize() const;

	static const int manufacturerHOffset = 3;
	static const int manufacturerLOffset = 4;
	static const int deviceTypeHOffset = 5;
	static const int deviceTypeLOffset = 6;
	static const int versionOffset = 7;
	static const int appPeiTypeOffset = 9;
	static const int portADDROffset = 12;
	static const int runErrorOffset = 13;
	static const int assocTabPtrOffset = 17;
	static const int commsTabPtrOffset = 18;
	static const int addrTabSizeOffset = 22;
	static const int addrTabOffset = 23;

	virtual byte& manufacturerH() const { return userEepromData[manufacturerHOffset]; }
	virtual byte& manufacturerL() const { return userEepromData[manufacturerLOffset]; }
	virtual byte& deviceTypeH() const { return userEepromData[deviceTypeHOffset]; }
	virtual byte& deviceTypeL() const { return userEepromData[deviceTypeLOffset]; }
	virtual byte& version() const { return userEepromData[versionOffset]; }
	virtual byte& appPeiType() const { return userEepromData[appPeiTypeOffset]; }
	virtual byte& portADDR() const { return userEepromData[portADDROffset]; }
	virtual byte& runError() const { return userEepromData[runErrorOffset]; }
    virtual byte& addrTabSize() const { return userEepromData[addrTabSizeOffset]; }
    virtual byte& assocTabPtr() const { return userEepromData[assocTabPtrOffset]; }
    virtual byte* addrTab() const { return &userEepromData[addrTabOffset]; }
    virtual byte& commsTabPtr() const { return userEepromData[commsTabPtrOffset]; };

protected:
	UserEepromBCU1(Bus *bus, int start, int size) : UserEeprom(bus, start, size) {};

#if 0
	union __attribute__ ((aligned (4)))
	{
		struct {
		0	byte optionReg;      //!< 0x0100: EEPROM option register
		1	byte manuDataH;      //!< 0x0101: Manufacturing data high byte
		2	byte manuDataL;      //!< 0x0102: Manufacturing data low byte
		3	byte manufacturerH;  //!< 0x0103: Software manufacturer high byte
		4	byte manufacturerL;  //!< 0x0104: Software manufacturer low byte
		5	byte deviceTypeH;    //!< 0x0105: Device type high byte
		6	byte deviceTypeL;    //!< 0x0106: Device type low byte
		7	byte version;        //!< 0x0107: Software version
		8	byte checkLimit;     //!< 0x0108: EEPROM check limit
		9	byte appPeiType;     //!< 0x0109: PEI type that the application program requires
		10	byte syncRate;       //!< 0x010a: Baud rate for serial synchronous PEI
		11	byte portCDDR;       //!< 0x010b: Port C DDR settings (PEI type 17)
		12	byte portADDR;       //!< 0x010c: Port A DDR settings
		13	byte runError;       //!< 0x010d: Runtime error flags
		14	byte routeCnt;       //!< 0x010e: Routing count constant
		15	byte maxRetransmit;  //!< 0x010f: INAK and BUSY retransmit limit
		16	byte confDesc;       //!< 0x0110: Configuration descriptor
		17	byte assocTabPtr;    //!< 0x0111: Low byte of the pointer to association table (BCU1 only)
		18	byte commsTabPtr;    //!< 0x0112: Low byte of the pointer to communication objects table (BCU1 only)
		19	byte usrInitPtr;     //!< 0x0113: Low byte of the pointer to user initialization function (BCU1 only)
		20	byte usrProgPtr;     //!< 0x0114: Low byte of the pointer to user program function (BCU1 only)
		21	byte usrSavePtr;     //!< 0x0115: Low byte of the pointer to user save function (BCU1 only)
		22	byte addrTabSize;    //!< 0x0116: Size of the address table
		23	byte addrTab[2];     //!< 0x0117+: Address table, 2 bytes per entry. Real array size is addrTabSize*2
		25	byte user[220];      //!< 0x0116: User EEPROM: 220 bytes (BCU1)
		245	byte checksum;       //!< 0x01ff: EEPROM checksum (BCU1 only)
		};
		byte userEepromData[156];
	} data;

    virtual int userEepromStart() const;
    virtual int userEepromSize() const;
    virtual byte* userEepromData();
#endif
};

inline int UserEepromBCU1::userEepromFlashSize() const
{
	return userEepromSize;
}

#endif
