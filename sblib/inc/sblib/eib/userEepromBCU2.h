#ifndef sblib_usereeprom_bcu2_h
#define sblib_usereeprom_bcu2_h

#include <sblib/eib/userEeprom.h>

/**
 * The user EEPROM.
 *
 * The user EEPROM can be accessed by name, like userEeprom.status and as an array, like
 * userEeprom[addr]. Please note that the start address of the EEPROM is subtracted. That means
 * userEeprom[0x107] is the correct address for userEeprom.version; not userEeprom[0x07].
 */
class UserEepromBCU2 : public UserEepromBCU1
{
public:
	UserEepromBCU2(Bus* bus) : UserEepromBCU1(bus, 0x100, 1024) {};
	virtual ~UserEepromBCU2() = default;

    virtual int userEepromFlashSize() const;

	static const int appTypeOffset = 21;
	static const int loadStateOffset = 880;
	static const int addrTabAddrOffset = 888;
	static const int assocTabAddrOffset = 890;
	static const int commsTabAddrOffset = 892;
    static const int commsSeg0AddrOffset = 894;
    static const int commsSeg1AddrOffset = 894;
	static const int eibObjAddrOffset = 898;
	static const int eibObjCountOffset = 900;
	static const int serviceControlOffset = 902;
	static const int serialOffset = 906;
	static const int orderOffset = 912;
	static const int addrTabMcbOffset = 932;
	static const int assocTabMcbOffset = 940;
	static const int commsTabMcbOffset = 948;
    static const int eibObjMcbOffset = 956;
    static const int eibObjVerOffset = 970;
	static const int commsSeg0VerOffset = 975;

	virtual const int serialSize() const { return 6; };

	virtual byte* loadState() const { return &userEepromData[loadStateOffset]; }
	virtual byte& appType() const { return userEepromData[appTypeOffset]; }
	virtual word& commsTabAddr() const { return *(word*)&userEepromData[commsTabAddrOffset]; }
	virtual word& commsSeg0Addr() const { return *(word*)&userEepromData[commsSeg0AddrOffset]; }
	virtual word& commsSeg1Addr() const { return *(word*)&userEepromData[commsSeg1AddrOffset]; }
	virtual word& eibObjAddr() const { return *(word*)&userEepromData[eibObjAddrOffset]; }
	virtual byte& eibObjCount() const { return userEepromData[eibObjCountOffset]; }
	virtual word& addrTabAddr() const { return *(word*)&userEepromData[addrTabAddrOffset]; }
	virtual word& assocTabAddr() const { return *(word*)&userEepromData[assocTabAddrOffset]; }
	virtual byte* serial() const { return &userEepromData[serialOffset]; }

	//virtual byte& peiType() const { return userEepromData[peiTypeOffset]; }

protected:
	UserEepromBCU2(Bus* bus, int start, int size) : UserEepromBCU1(bus, start, size) {};

#if 0
	union __attribute__ ((aligned (4)))
	{
		struct
		{
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
		18	byte usrInitPtr;     //!< 0x0113: Low byte of the pointer to user initialization function (BCU1 only)
		20	byte usrProgPtr;     //!< 0x0114: Low byte of the pointer to user program function (BCU1 only)
		21	byte appType;        //!< 0x0115: Application program type: 0=BCU2, else BCU1
		22	byte addrTabSize;    //!< 0x0116: Size of the address table
		23	byte addrTab[2];     //!< 0x0117+:Address table, 2 bytes per entry. Real array size is addrTabSize*2
		25	byte user[855];      //!< 0x0119+:User EEPROM: 856 bytes (BCU2)
								 //!< ------  System EEPROM below (BCU2)
		880	byte loadState[INTERFACE_OBJECT_COUNT];   //!< 0x0470: Load state of the system interface objects
		888	word addrTabAddr;    //!< 0x0478: Address of the address table
		890	word assocTabAddr;   //!< 0x047a: Address of the association table
		892	word commsTabAddr;   //!< 0x047c: Address of the communication object table
		894	word commsSeg0Addr;  //!< 0x047e: Address of communication object memory segment 0 // TODO needs implementation, see handleTaskCtrl2(...) in properties.cpp
		896	word commsSeg1Addr;  //!< 0x0480: Address of communication object memory segment 1 // TODO needs implementation, see handleTaskCtrl2(...) in properties.cpp
		898	word eibObjAddr;     //!< 0x047c: Address of the application program EIB objects, 0 if unused. // TODO needs implementation, see handleTaskCtrl1(...) in properties.cpp
		900	byte eibObjCount;    //!< 0x047e: Number of application program EIB objects. // TODO needs implementation, see handleTaskCtrl1(...) in properties.cpp
		901	byte padding1;       //!< 0x047f: Padding
		902	word serviceControl; //!< 0x0480: Service control
		904	word padding2;       //!< 0x0482: Padding
		906	byte serial[6];      //!< 0x0484: Hardware serial number (4 byte aligned)
		912	byte order[10];      //!< 0x048a: Hardware Type
		922	byte orderInfo[10];  //!< 0x0494: Ordering information
		932	byte addrTabMcb[8];
		940	byte assocTabMcb[8];
		948	byte commsTabMcb[8];
		956	byte eibObjMcb[8];
		964	byte commsSeg0Mcb[8];
		972	byte eibObjVer[5];      //!< Application programm 1 version
		977	byte commsSeg0Ver[5];   //!< Application programm 2 version
		982};
		byte userEepromData[1024];
	} data;

	virtual int userEepromStart() const;
	virtual int userEepromSize() const;
    virtual byte* userEepromData();
#endif
};

inline int UserEepromBCU2::userEepromFlashSize() const
{
	return userEepromSize;
}

#endif
