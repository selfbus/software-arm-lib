#ifndef sblib_usereeprom_bcu2_h
#define sblib_usereeprom_bcu2_h

#include <sblib/eib/userEepromBCU1.h>

/**
 * The BCU 2 user EEPROM
 */
class UserEepromBCU2 : public UserEepromBCU1
{
public:
	UserEepromBCU2(BcuBase* bcu) : UserEepromBCU1(bcu, 0x100, 1024, 1024) {};
	~UserEepromBCU2() = default;

	static const int appTypeOffset = 21;        //!< 0x0115: \todo Application program type: 0=BCU2, else BCU1

	static const int loadStateOffset = 880;     //!< 0x0470: Load state of the system interface objects
	static const int addrTabAddrOffset = 888;   //!< 0x0478: Address of the address table
	static const int assocTabAddrOffset = 890;  //!< 0x047a: Address of the association table
	static const int commsTabAddrOffset = 892;  //!< 0x047c: Address of the communication object table
    static const int commsSeg0AddrOffset = 894; //!< 0x047e: Address of communication object memory segment 0 // TODO needs implementation, see handleTaskCtrl2(...) in properties.cpp
    static const int commsSeg1AddrOffset = 896; //!< 0x0480: Address of communication object memory segment 1 // TODO needs implementation, see handleTaskCtrl2(...) in properties.cpp
	static const int eibObjAddrOffset = 898;    //!< 0x0482: Address of the application program EIB objects, 0 if unused. // TODO needs implementation, see handleTaskCtrl1(...) in properties.cpp
	static const int eibObjCountOffset = 900;   //!< 0x0484: Number of application program EIB objects. // TODO needs implementation, see handleTaskCtrl1(...) in properties.cpp
	static const int padding1Offset = 0x0385;   //!< 0x0485: Padding 1
	static const int serviceControlOffset = 902;//!< 0x0486: Service control
	static const int padding2Offset = 0x0388;   //!< 0x0488: Padding 2
	static const int serialOffset = 906;        //!< 0x048A-0x48f: Hardware serial number (4 byte aligned)

	static const int orderOffset = 912;         //!< 0x0490-0x0499: Hardware Type
	static const int orderInfoOffset = 0x039A;  //!< 0x049A-0x04A3: Ordering information

	///\todo why here, isn't this system b specific?
	static const int addrTabMcbOffset = 932;    //!< 0x04A4-0x04AB:
	static const int assocTabMcbOffset = 940;   //!< 0x04AC-0x04B3:
	static const int commsTabMcbOffset = 948;   //!< 0x04B4-0x04BB:
    static const int eibObjMcbOffset = 956;     //!< 0x04BC-0x04C3:
    static const int commsSeg0McbOffset = 0x03C3; //!< 0x04C4-0x04CB:
    static const int eibObjVerOffset = 972;     //!< 0x04CC-0x04D0: Application program 1 version
	static const int commsSeg0VerOffset = 977;  //!< 0x04D1-0x04D5: Application program 2 version
	// end todo

	virtual int serialSize() const { return 6; };
	virtual int orderSize() const { return 10; };
	virtual int orderInfoSize() const { return 10; };

	virtual byte& appType() const { return userEepromData[appTypeOffset]; }
	virtual byte* loadState() const { return &userEepromData[loadStateOffset]; }
	virtual word& commsTabAddr() const { return *(word*)&userEepromData[commsTabAddrOffset]; }
	virtual word& commsSeg0Addr() const { return *(word*)&userEepromData[commsSeg0AddrOffset]; }
	virtual word& commsSeg1Addr() const { return *(word*)&userEepromData[commsSeg1AddrOffset]; }
	virtual word& eibObjAddr() const { return *(word*)&userEepromData[eibObjAddrOffset]; }
	virtual byte& eibObjCount() const { return userEepromData[eibObjCountOffset]; }
	virtual word& addrTabAddr() const { return *(word*)&userEepromData[addrTabAddrOffset]; }
	virtual word& assocTabAddr() const { return *(word*)&userEepromData[assocTabAddrOffset]; }
	virtual byte* serial() const { return &userEepromData[serialOffset]; }
	virtual byte* order() const { return &userEepromData[orderOffset]; }

	virtual byte& padding1() const { return userEepromData[padding1Offset]; }
	virtual byte& serviceControl() const { return userEepromData[serviceControlOffset]; }
	virtual byte& padding2() const { return userEepromData[padding2Offset]; }
	virtual byte* orderInfo() const { return &userEepromData[orderInfoOffset]; }

	//virtual byte& peiType() const { return userEepromData[peiTypeOffset]; }

protected:
	UserEepromBCU2(BcuBase* bcu, unsigned int start, unsigned int size, unsigned int flashSize) : UserEepromBCU1(bcu, start, size, flashSize) {};
};

#endif /*sblib_usereeprom_bcu2_h*/
