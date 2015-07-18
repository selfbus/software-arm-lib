/*
 * sblib_default_objects.h
 *
 *  Created on: 15.07.2015
 *      Author: glueck
 */

#ifndef SBLIB_DEFAULT_OBJECTS_H_
#define SBLIB_DEFAULT_OBJECTS_H_

#include <sblib/eib.h>

static BCU _bcu = BCU();
BCU_Base * bcu = &_bcu;

// The EIB bus access object
#if defined (__LPC11XX__)
Bus bus(timer16_1, PIO1_8, PIO1_9, CAP0, MAT0);
#elif defined (__LPC11UXX__)
Bus bus(timer16_1, PIO0_20, PIO0_21, CAP0, MAT0);
#endif

#endif /* SBLIB_DEFAULT_OBJECTS_H_ */
