#ifndef sblib_usereeprom_systemb_h
#define sblib_usereeprom_systemb_h

#include <sblib/eib/userEeprom.h>

/**
 * The user EEPROM.
 *
 * The user EEPROM can be accessed by name, like userEeprom.status and as an array, like
 * userEeprom[addr]. Please note that the start address of the EEPROM is subtracted. That means
 * userEeprom[0x107] is the correct address for userEeprom.version; not userEeprom[0x07].
 */
class UserEepromSYSTEMB : public UserEepromBCU2
{
public:
	UserEepromSYSTEMB(Bus* bus) : UserEepromBCU2(bus, 0x3300, 3072) {};
	virtual ~UserEepromSYSTEMB() = default;

    virtual int userEepromFlashSize() const;

	static const int addrTabAddrOffset = 33;
	static const int assocTabAddrOffset = 35;
	static const int addrTabMcbOffset = 77;
	static const int assocTabMcbOffset = 85;
	static const int commsTabMcbOffset = 93;
	static const int commsSeg0McbOffset = 109;
	static const int eibObjMcbOffset = 101;

	virtual word& addrTabAddr() const { return *(word*)&userEepromData[addrTabAddrOffset]; }
	virtual word& assocTabAddr() const { return *(word*)&userEepromData[assocTabAddrOffset]; }
	virtual byte* addrTabMcb() const { return &userEepromData[addrTabMcbOffset]; }
	virtual byte* assocTabMcb() const { return &userEepromData[assocTabMcbOffset]; }
	virtual byte* commsTabMcb() const { return &userEepromData[commsTabMcbOffset]; }
	virtual byte* commsSeg0Mcb() const { return &userEepromData[commsSeg0McbOffset]; }
	virtual byte* eibObjMcb() const { return &userEepromData[eibObjMcbOffset]; }

protected:
	UserEepromSYSTEMB(Bus* bus, int start, int size) : UserEepromBCU2(bus, start, size) {};

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
		21	byte appType;        //!< 0x0115: Application program type: 0=BCU2, else BCU1
		22	byte addrTabSize;    //!< 0x0116: Size of the address table
		23	byte addrTab[2];     //!< 0x0117+:Address table, 2 bytes per entry. Real array size is addrTabSize*2
								 //!< ------  System EEPROM below (BCU2)
		25	byte loadState[INTERFACE_OBJECT_COUNT];   //!< 0x0470: Load state of the system interface objects
		33	word addrTabAddr;    //!< 0x0478: Address of the address table
		35	word assocTabAddr;   //!< 0x047a: Address of the association table
		37	word commsTabAddr;   //!< 0x047c: Address of the communication object table
		39	word commsSeg0Addr;  //!< 0x047e: Address of communication object memory segment 0 // TODO needs implementation, see handleTaskCtrl2(...) in properties.cpp
		41	word commsSeg1Addr;  //!< 0x0480: Address of communication object memory segment 1 // TODO needs implementation, see handleTaskCtrl2(...) in properties.cpp
		43	word eibObjAddr;     //!< 0x047c: Address of the application program EIB objects, 0 if unused. // TODO needs implementation, see handleTaskCtrl1(...) in properties.cpp
		45	byte eibObjCount;    //!< 0x047e: Number of application program EIB objects. // TODO needs implementation, see handleTaskCtrl1(...) in properties.cpp
		46	byte padding1;       //!< 0x047f: Padding
		47	word serviceControl; //!< 0x0480: Service control
		49	word padding2;       //!< 0x0482: Padding
		51	byte serial[6];      //!< 0x0484: Hardware serial number (4 byte aligned)
		57	byte order[10];      //!< 0x048a: Hardware Type
		67	byte orderInfo[10];  //!< 0x0494: Ordering information
		77	byte addrTabMcb[8];
		85	byte assocTabMcb[8];
		93	byte commsTabMcb[8];
		101	byte eibObjMcb[8];
		109	byte commsSeg0Mcb[8];
		117	byte eibObjVer[5];      //!< Application programm 1 version
		122	byte commsSeg0Ver[5];   //!< Application programm 2 version
		};
		byte userEepromData[3072];
	} data;

	virtual int userEepromStart() const;
	virtual int userEepromSize() const;
    virtual int userEepromFlashSize() const;
    virtual byte* userEepromData();
#endif
};

inline int UserEepromSYSTEMB::userEepromFlashSize() const
{
	return 4096;
}

#endif
