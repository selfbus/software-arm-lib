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
#ifndef sblib_com_objects_BCU1_h
#define sblib_com_objects_BCU1_h

#include <sblib/eib/com_objects.h>
#include <sblib/eib/typesBCU1.h>

class BcuDefault;

/**
 * Group Object Table - Realisation Type 1
 * @details KNX Spec. 2.1 3/5/1 4.12.2 p. 168ff
 *
 */
class ComObjectsBCU1 : public ComObjects
{
public:
    ComObjectsBCU1() = delete;
	ComObjectsBCU1(BcuDefault* bcuInstance) : ComObjects((BcuBase*)bcuInstance) {}
	~ComObjectsBCU1() = default;

	virtual const ComConfig& objectConfig(int objno) override;
	virtual int objectSize(int objno) override;
	virtual byte* objectValuePtr(int objno) override;
	virtual void processGroupTelegram(uint16_t addr, int apci, byte* tel, int trg_objno) override;
	virtual byte* objectConfigTable() override;
	virtual byte* objectFlagsTable() override;

	const ComConfigBCU1* objectConfigBCU1(int objno); ///\todo make protected again after ramLocation fix, see setup.cpp fixRamLoc(.) of 4sense-bcu1
protected:
	// The size of the object types BIT_7...VARDATA in bytes
	const byte objectTypeSizes[10] = { 1, 1, 2, 3, 4, 6, 8, 10, 14, 15 };

	virtual const byte* getObjectTypeSizes() override;
};

#endif /*sblib_com_objects_BCU1_h*/
