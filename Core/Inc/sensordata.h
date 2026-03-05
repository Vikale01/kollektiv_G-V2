#ifndef __SENSORDATA_H
#define __SENSORDATA_H

#include "main.h"

/* Structs -----------------------------------------*/

// // Data from the BMI should be saved more often
// typedef struct 
// {
//     uint16_t gx;
//     uint16_t gy;
//     uint16_t gz;
//     uint16_t ax;
//     uint16_t ay;
//     uint16_t az;
// }fast_data_t;

// // Data from the BMI should be saved less than the BMI
// typedef struct
// {
//     uint8_t identifier_start;
//     uint8_t bmeData[8];
//     uint16_t year;
//     uint8_t month;
//     uint8_t day;
//     uint8_t hour;
//     uint8_t min;
//     uint8_t sec;
//     uint32_t longitude;
//     uint32_t latitude;
//     uint8_t identifier_end;
// }slow_data_t;

/* Defines -----------------------------------------*/



/* Variables -----------------------------------------*/


/* Functions -----------------------------------------*/
void packageDataToMem(void);

void sendPackageToMem(void);

void readMemSendBle(void);

#endif