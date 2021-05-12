/*
 * update.h
 *
 *  Created on: May 24, 2020
 *      Author: Stefan Haller
 */

#ifndef UPDATE_H_
#define UPDATE_H_

// Handle delayed restart request
bool restartRequestExpired(void);
unsigned int request_flashWrite(unsigned char *, bool);


#endif /* UPDATE_H_ */
