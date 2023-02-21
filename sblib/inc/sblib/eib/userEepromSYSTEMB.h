#ifndef sblib_usereeprom_systemb_h
#define sblib_usereeprom_systemb_h

#include <sblib/eib/userEepromMASK0701.h>

/**
 * The BCU SYSTEM B user EEPROM
 */
class UserEepromSYSTEMB : public UserEepromMASK0701
{
public:
    ///\todo check start of 0x3300, maybe the same reason like for 0x0701 -> -0x100 is chosen to avoid address-offset calculations to a BCU1
	UserEepromSYSTEMB() : UserEepromMASK0701(0x3300, 3072, 4096) {};
	~UserEepromSYSTEMB() = default;

	static const int addrTabAddrOffset  = 0x21; //!< 0x3321-0x3322
	static const int assocTabAddrOffset = 0x23; //!< 0x3323-0x3324

	static const int addrTabMcbOffset   = 0x4D; //!< 0x334D-0x3354
	static const int assocTabMcbOffset  = 0x55; //!< 0x3355-0x335C
	static const int commsTabMcbOffset  = 0x5D; //!< 0x335D-0x3364
	static const int eibObjMcbOffset    = 0x65; //!< 0x3365-0x336C
	static const int commsSeg0McbOffset = 0x6D; //!< 0x336D-0x3374
    static const int eibObjVerOffset    = 0x75; //!< 0x3375-0x3379 Application program 1 version
    static const int commsSeg0VerOffset = 0x7A; //!< 0x337A-0x337E Application program 2 version

	virtual word& addrTabAddr() const override { return *(word*)&userEepromData[addrTabAddrOffset]; }
	virtual word& assocTabAddr() const override { return *(word*)&userEepromData[assocTabAddrOffset]; }
	virtual byte* addrTabMcb() const { return &userEepromData[addrTabMcbOffset]; }
	virtual byte* assocTabMcb() const { return &userEepromData[assocTabMcbOffset]; }
	virtual byte* commsTabMcb() const { return &userEepromData[commsTabMcbOffset]; }
	virtual byte* commsSeg0Mcb() const { return &userEepromData[commsSeg0McbOffset]; }
	virtual byte* eibObjMcb() const { return &userEepromData[eibObjMcbOffset]; }

protected:
	UserEepromSYSTEMB(unsigned int start, unsigned int size, unsigned int flashSize) : UserEepromMASK0701(start, size, flashSize) {};
};

#endif /*sblib_usereeprom_systemb_h*/
