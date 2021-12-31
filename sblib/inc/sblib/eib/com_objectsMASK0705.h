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
#ifndef sblib_com_objects_MASK0705_h
#define sblib_com_objects_MASK0705_h

#include <sblib/eib/com_objectsMASK0701.h>

class BcuBase;

class ComObjectsMASK0705 : public ComObjectsMASK0701
{
public:
	ComObjectsMASK0705(BcuBase* bcuInstance) : ComObjectsMASK0701(bcuInstance) {}
	~ComObjectsMASK0705() = default;
};

#endif /*sblib_com_objects_h*/
