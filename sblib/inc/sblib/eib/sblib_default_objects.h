/*
 * sblib_default_objects.h
 *
 *  Created on: 15.07.2015
 *      Author: glueck
 */

#ifndef SBLIB_DEFAULT_OBJECTS_H_
#define SBLIB_DEFAULT_OBJECTS_H_

#include <sblib/eib.h>
#include <sblib/io_pin_names.h>
static BCU _bcu = BCU();
BcuBase& bcu = _bcu;

// The EIB bus access object
Bus bus(timer16_1, PIN_EIB_RX, PIN_EIB_TX, CAP0, MAT0);

#endif /* SBLIB_DEFAULT_OBJECTS_H_ */
