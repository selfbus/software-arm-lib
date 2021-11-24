/*
 *  com_objects.h - EIB Communication objects.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *
 **  last update: Jan. 2021 Horst Rauch
 *  			 - added some explanation on the functions of this interface between
 *  			   the user application and the communication functions of the knx bus.
 *  			 - changed some function for the transmission of object from
 *  			 user appl. to knx bus and vice versa as there was an endless loop in some cases
 *
 *  This program is the main interface for the communication of the user application of the knx bus device,
 *  based on the functions of a BCUx or BIMx provided by the complete library.
 *  The basic interface as describe in knx spec 3.3.7, 3.4.1, 3.5.1 is hidden by some functions to write and read
 *  certain object types. We include some parts of these specs for a better understanding of the interface functions.
 *
 *  The interface between the user application and the knx system is based on so called GroupObjects
 *  (GO, also communication objects). The GroupObject Server act as an interface between the user application and
 *  the application layer of the knx stack. As the (communication) stack layers and the knx bus system has no information
 *  on the type and characteristics of a GO from the user application, data is provided by the
 *  user appl. and partly configured by the ETS.
 *
 *  The detailed handling of the GO flags and configuration is hidden within the module from user application.
 *  Only ObjectRead, ObjectWrite and ObjectUpdate are mainly used by the user application.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_com_objects_SYSTEMB_h
#define sblib_com_objects_SYSTEMB_h

#include <sblib/eib/com_objectsBCU2.h>

class SYSTEMB;

class ComObjectsSYSTEMB : public ComObjectsBCU2
{
public:
	ComObjectsSYSTEMB(SYSTEMB* bcuInstance) : ComObjectsBCU2((BCU2*)bcuInstance), bcu(bcuInstance) {}
	virtual ~ComObjectsSYSTEMB() = default;

	virtual inline const ComConfig& objectConfig(int objno);
	SYSTEMB* bcu;

protected:
	// The size of the object types BIT_7...VARDATA in bytes
	const byte objectTypeSizes[15] = { 1, 1, 2, 3, 4, 6, 8, 10, 14, 5, 7, 9, 11, 12, 13};

	virtual const byte* getObjectTypeSizes();

	virtual int objectSize(int objno);
	virtual byte* objectValuePtr(int objno);
	virtual void processGroupTelegram(int addr, int apci, byte* tel, int trg_objno);
	virtual byte* objectConfigTable();
	virtual byte* objectFlagsTable();

};

#endif /*sblib_com_objects_h*/
