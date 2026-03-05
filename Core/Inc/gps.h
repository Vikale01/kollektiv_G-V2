// NEO_M9N.h – byt namn till NEO_M9V.h om du vill
#ifndef __NEO_M9V_H
#define __NEO_M9V_H

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

#define RX_BUFFER_LEN  100

#define CHECKSUM_SP     2
#define CHECKSUM_EP     97

// RÄTT ID för UBX-NAV-PVT (little-endian B5 62 01 07)
#define PVT_ID  0x070162B5UL

// RÄTTA positioner för UBX-NAV-PVT (92 byte payload) +6

#define ITOW_POS        6

#define YER_POS         10
#define MON_POS         12
#define DAY_POS         13
#define HOR_POS         14
#define MIN_POS         15
#define SEC_POS         16
#define NANO_POS        22

#define FIX_POS         26          // gpsFix
#define SAT_POS         29          // numSVö

#define LNG_POS         30          // lon (int32_t) → grader * 1e7
#define LAT_POS         34          // lat (int32_t)
#define HEIGHT_POS      38          // height above ellipsoid (mm)
#define MSL_POS         42          // height above mean sea level (mm)
#define HACC_POS        46          // horizontal accuracy (mm)
#define VACC_POS        50          // vertical accuracy (mm)

#define GSPEED_POS      66          //Speed
#define HEADMOT_POS     70          //Headmotion

#define COORD_FACTOR     1e-7f
#define ALTITUDE_FACTOR  0.001f     // mm → m

typedef struct {
    bool    newData;
    uint32_t ITOW;
    uint16_t year;
    uint8_t month, day, hour, minute;
    uint8_t second;
    uint8_t fixType;
    uint8_t numSV;
    float   lon;
    float   lat;
    float   heightMSL;
    float   hAcc;
    float   vAcc;
    float   VelN;
    float   VelE;
    float   VelD; 
    float   gSpeed;      
    float   headMot; 
    float   sAcc;    
    float   headAcc;  
    float   headVeh;  
} GPS_DATA_t;

extern volatile uint8_t gpsRxBuffer[RX_BUFFER_LEN];
extern GPS_DATA_t myGpsData;

extern uint8_t GPS_connected;

extern uint8_t GPSraw_data[15];

void GPS_Start(void);

#endif