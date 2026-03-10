/***********************************************************
 * @file GPS.c
 * @brief   U-blox UBX-NAV-PVT parser med DMA + IDLE-line
 * @author  Viktor Alexandersson & Niklas Christensen 
 * @date    2025-12-04
 ***********************************************************/
/* Includes -----------------------------------------------*/
#include "gps.h"

/* extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef handle_GPDMA1_Channel2;
 */

extern UART_HandleTypeDef hlpuart1;
extern DMA_HandleTypeDef hdma_lpuart1_rx;

/* Receive-buffer - 4-byte aligned for safe 32/64-bit access */
volatile uint8_t gpsRxBuffer[RX_BUFFER_LEN] __attribute__((aligned(4)));

/* Parsed GPS-data structure*/
GPS_DATA_t myGpsData = {0};
uint8_t GPS_connected = 0;

uint8_t logoSwitched = 0;


uint8_t start_pos = 0;

/**
 * @brief  Verify UBX checksum
 * @return True if checksum is valid
 */
static uint8_t checkSum(volatile uint8_t *p)
{
    uint8_t ck_a = 0, ck_b = 0;

    /* Checking if buffer received is correctly structured */
    for (int i = CHECKSUM_SP; i <= CHECKSUM_EP; i++) {
        ck_a += p[i];
        ck_b += ck_a;
    }
    if((ck_a == p[98]) && (ck_b == p[99])) return 1;
    return 0;
}

/**
 * @brief  Parse a complete UBX-NAV-PVT message and populate myGpsData
 * @note   Called from UART RX callback when a full message has been received
 */
void processGPS(volatile uint8_t *p)
{   

    /* Header validation */
    if(p[0] == 0xB5 && p[1] == 0x62 && p[2] == 0x01 && p[3] == 0x07){

        /* Checksum validation */
        //if(checkSum(p) == 1){

        myGpsData.newData   = true;

        /* Time & Date */
        myGpsData.year      = p[YER_POS] | (p[YER_POS+1] << 8);
        myGpsData.month     = p[MON_POS];
        myGpsData.day       = p[DAY_POS];
        myGpsData.hour      = p[HOR_POS];
        myGpsData.minute    = p[MIN_POS];
        myGpsData.ITOW      = p[ITOW_POS] | (p[ITOW_POS+1] << 8) | (p[ITOW_POS+2] << 16) | (p[ITOW_POS+3] << 24);
                    
        if (p[17] & 0x01) // Valid flag
        {
            myGpsData.second = p[SEC_POS];
        }
        else
        {
            myGpsData.second = myGpsData.ITOW / 1000 % 60;
        }

        /* Fix Quality */
        myGpsData.fixType   = p[FIX_POS];
        myGpsData.numSV     = p[SAT_POS];

        /* Position */
        myGpsData.lon       = (float)*(int32_t *)&p[LNG_POS] * COORD_FACTOR;
        myGpsData.lat       = (float)*(int32_t *)&p[LAT_POS] * COORD_FACTOR;

        /* Speed & Heading */
        myGpsData.gSpeed    = (float)*(int32_t *)&p[GSPEED_POS];
        myGpsData.headMot   = p[HEADMOT_POS];

        if(myGpsData.numSV >= 3)
        {
            GPS_connected = 1;
        }
        else{
            GPS_connected = 0;
            }
        //}

        GPSraw_data[0] =     p[YER_POS];
        GPSraw_data[1] =     p[YER_POS + 1];
        GPSraw_data[2] =     p[MON_POS];
        GPSraw_data[3] =     p[DAY_POS];
        GPSraw_data[4] =     p[HOR_POS];
        GPSraw_data[5] =     p[MIN_POS];
        GPSraw_data[6] =     p[SEC_POS];

        GPSraw_data[7] =     p[LNG_POS];
        GPSraw_data[8] =     p[LNG_POS + 1];
        GPSraw_data[9] =     p[LNG_POS + 2];
        GPSraw_data[10] =    p[LNG_POS + 3];

        GPSraw_data[11] =    p[LAT_POS];
        GPSraw_data[12] =    p[LAT_POS + 1];
        GPSraw_data[13] =    p[LAT_POS + 2];
        GPSraw_data[14] =    p[LAT_POS + 3];
    }    
}

/**
 * @brief  Start continuous GPS reception using DMA
 */
void GPS_Start(void)
{
    HAL_HalfDuplex_EnableReceiver(&hlpuart1);

    if (HAL_UART_Receive_DMA(&hlpuart1, (uint8_t*)gpsRxBuffer, RX_BUFFER_LEN) != HAL_OK)
    {
        Error_Handler();
    }
    
    __HAL_DMA_DISABLE_IT(&hdma_lpuart1_rx, DMA_IT_HT); 

}

void Find_start(void)
{
    for (int i = 0; i < RX_BUFFER_LEN - 1; i++)
    {
        if (gpsRxBuffer[i] == 0xB5 && gpsRxBuffer[i + 1] == 0x62)
        {
            processGPS(&gpsRxBuffer[i]);    
            break;
        }
    }
}

/**
 * @brief  UART RX callback – triggered when line is detected (frame complete)
 * @param  huart  UART handle
 * @param  Size   Number of bytes received since last start (not strictly needed)
 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &hlpuart1)
    {
        Find_start();
        HAL_UART_Receive_DMA(&hlpuart1, (uint8_t*)gpsRxBuffer, RX_BUFFER_LEN);      
    }
}

// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
// {
//     if (huart == &hlpuart1)
//     {
//         processGPS();
//         HAL_UARTEx_ReceiveToIdle_DMA(&hlpuart1, (uint8_t*)gpsRxBuffer, RX_BUFFER_LEN);
//     }
// }



void gpsLogo()
{
    if (GPS_connected && logoSwitched == 0)
    {
        Oled_Draw1BitImage(107,0, icon_gps_20x20, ICON_GPS_W, ICON_GPS_H, 0x0F);
        SSD1327_UpdateArea(107, 0, 127, 20, NULL);
        logoSwitched = 1;
    }
    else if (!GPS_connected)
    {
        Oled_Draw1BitImage(107,0, icon_gps_20x20, ICON_GPS_W, ICON_GPS_H, 0x05);
        SSD1327_UpdateArea(107, 0, 127, 20, NULL);
        logoSwitched = 0;
    }
}
