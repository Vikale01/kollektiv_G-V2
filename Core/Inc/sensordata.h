#ifndef __SENSORDATA_H
#define __SENSORDATA_H

#include "main.h"

/* Structs -----------------------------------------*/

typedef struct {
    uint32_t pageNumber;
    uint8_t pageCounter;
    uint16_t sectorCounter;
    uint32_t sequence;
} FlashIndex_t;



/* Defines -----------------------------------------*/

#define INDEX_SECTOR 15623
#define INDEX_ADDRESS ((INDEX_SECTOR * SECTION_SIZE) + 0)

/* Variables -----------------------------------------*/


/* Functions -----------------------------------------*/
void packageDataToMem(void);

void sendPackageToMem(void);

void readMemSendBle(void);

void sensordata_init(void);

#endif