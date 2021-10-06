//
// Created by Mario Theodoridis on 21.05.20.
//

#ifndef INC_4SENSE_LOGGER_H
#define INC_4SENSE_LOGGER_H

void initLogger(int txPin = PIO1_7, int rxPin = PIO1_6);
void serPrintf(const char *fmt, ...);

#endif //INC_4SENSE_LOGGER_H
