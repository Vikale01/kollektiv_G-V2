#ifndef __SENSORDATA_H
#define __SENSORDATA_H

#include "main.h"


void packageDataToMem(void);

void sendPackageToMem(void);

void findStartPos(void);


void sendDataStepByStep(void);

void sendDataUSB(void);

void sendDataUSB_Live(uint8_t buffer[12]);

#endif