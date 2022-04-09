#ifndef sblib_usereeprom_bcu1_h
#define sblib_usereeprom_bcu1_h

#include <sblib/eib/userEeprom.h>

/**
 * The BCU 1 user EEPROM
 */
class UserEepromBCU1 : public UserEeprom
{
public:
	UserEepromBCU1(BcuBase *bcu) : UserEeprom(bcu, 0x100, 256, 256) {};
	~UserEepromBCU1() = default;

	static const int optionRegOffset = 0;       //!< 0x0100: EEPROM option register
	static const int manuDataHOffset = 1;       //!< 0x0101: Manufacturing data high byte
	static const int manuDataLOffset = 2;       //!< 0x0102: Manufacturing data low byte
	static const int manufacturerHOffset = 3;   //!< 0x0103: Software manufacturer high byte
	static const int manufacturerLOffset = 4;   //!< 0x0104: Software manufacturer low byte
	static const int deviceTypeHOffset = 5;     //!< 0x0105: Device type high byte
	static const int deviceTypeLOffset = 6;     //!< 0x0106: Device type low byte
	static const int versionOffset = 7;         //!< 0x0107: Software version
	static const int checkLimitOffset = 8;      //!< 0x0108: EEPROM check limit
	static const int appPeiTypeOffset = 9;      //!< 0x0109: PEI type that the application program requires
	static const int syncRateOffset = 0x0a;     //!< 0x010a: Baud rate for serial synchronous PEI
	static const int portCDDROffset = 0x0b;     //!< 0x010b: Port C DDR settings (PEI type 17)
	static const int portADDROffset = 12;       //!< 0x010c: Port A DDR settings
	static const int runErrorOffset = 13;       //!< 0x010d: Runtime error flags
	static const int routeCntOffset = 0x0e;     //!< 0x010e: Routing count constant
	static const int maxRetransmitOffset = 0x0f;//!< 0x010f: INAK and BUSY retransmit limit
	static const int confDescOffset = 0x10;     //!< 0x0110: Configuration descriptor
	static const int assocTabPtrOffset = 17;    //!< 0x0111: Pointer to association table
	static const int commsTabPtrOffset = 18;    //!< 0x0112: Pointer to communication objects table
	static const int usrInitPtrOffset = 0x13;   //!< 0x0113: Pointer to user initialization function
	static const int usrProgPtrOffset = 0x14;   //!< 0x0114: Pointer to user program function
	static const int usrSavePtrOffset = 0x15;   //!< 0x0115: Pointer to user save function (BCU1 only)
    static const int addrTabSizeOffset = 22;    //!< 0x0116: Size of the address table
    static const int addrTabOffset = 23;        //!< 0x0117+: Address table, 2 bytes per entry. Real array size is addrTabSize*2
	static const int user230bytesStartOffset = 0x19; //!< 0x0119: User EEPROM: 230 bytes (BCU1)
	static const int checksumOffset = 0xff;     //!< 0x01ff: EEPROM checksum (BCU1 only)

	virtual byte& optionReg() const override { return userEepromData[optionRegOffset]; }
	virtual byte& manuDataH() const override { return userEepromData[manuDataHOffset]; }
	virtual byte& manuDataL() const override { return userEepromData[manuDataLOffset]; }
	virtual byte& manufacturerH() const override { return userEepromData[manufacturerHOffset]; }
	virtual byte& manufacturerL() const override { return userEepromData[manufacturerLOffset]; }
	virtual byte& deviceTypeH() const override { return userEepromData[deviceTypeHOffset]; }
	virtual byte& deviceTypeL() const override { return userEepromData[deviceTypeLOffset]; }
	virtual byte& version() const override { return userEepromData[versionOffset]; }
	virtual byte& checkLimit() const override { return userEepromData[checkLimitOffset]; }
	virtual byte& appPeiType() const override { return userEepromData[appPeiTypeOffset]; }
	virtual byte& syncRate() const override { return userEepromData[syncRateOffset]; }
	virtual byte& portCDDR() const override { return userEepromData[portCDDROffset]; }
	virtual byte& portADDR() const override { return userEepromData[portADDROffset]; }
	virtual byte& runError() const override { return userEepromData[runErrorOffset]; }
	virtual byte& routeCnt() const override { return userEepromData[routeCntOffset]; }
	virtual byte& maxRetransmit() const override { return userEepromData[maxRetransmitOffset]; }
	virtual byte& confDesc() const override { return userEepromData[confDescOffset]; }
	virtual byte& assocTabPtr() const override { return userEepromData[assocTabPtrOffset]; }
	virtual byte& commsTabPtr() const override { return userEepromData[commsTabPtrOffset]; };
    virtual byte& usrInitPtr() const override { return userEepromData[usrInitPtrOffset]; }
    virtual byte& usrProgPtr() const override { return userEepromData[usrProgPtrOffset]; }
    virtual byte& usrSavePtr() const { return userEepromData[usrSavePtrOffset]; }
    virtual byte& addrTabSize() const override { return userEepromData[addrTabSizeOffset]; }
    virtual byte* addrTab() const override { return &userEepromData[addrTabOffset]; }
    virtual byte* user230bytesStart() const { return &userEepromData[user230bytesStartOffset]; }
    virtual byte& checksum() const { return userEepromData[checksumOffset]; }

protected:
	UserEepromBCU1(BcuBase *bcu, unsigned int start, unsigned int size, unsigned int flashSize) : UserEeprom(bcu, start, size, flashSize) {};
};

#endif /*sblib_usereeprom_bcu1_h*/
