/***********************************************************
 * @file    sensordata.c
 * @author  Viktor Alexandersson & Niklas Christensen 
 * @date    2025-02-27
 ***********************************************************/

/* Includes -----------------------------------------------*/
#include "sensordata.h"

/* Defines -----------------------------------------*/
#define BUFFER_SIZE 256
#define SECTION_SIZE 4096

/* Variables -----------------------------------------------*/
uint8_t bufferPointer = 0;
uint8_t dataCounter = 0;

uint8_t pageCounter = 0;
uint16_t sectorCounter = 0;

bool header_sent = false;
bool bufferFull = false; 

uint8_t headerBuffer[25];

uint8_t GPSraw_data[15]; 
uint8_t BMEraw_data[8]; 

uint8_t BMIraw_data[12]; 

uint16_t bleSector = 0;
uint8_t ble_page = 0;

uint8_t buffer[BUFFER_SIZE];
uint8_t readbuffer[BUFFER_SIZE];

uint16_t pageNumber = 0;


/* Private functions -----------------------------------------------*/

static void sendPage_ble(uint8_t pageBuffer[256]) 
{ 
    tBleStatus status;

    // send first 128 bytes
    do {
        status = Custom_STM_App_Update_Char_Variable_Length(CUSTOM_STM_DP, pageBuffer, 128);
        if(status != BLE_STATUS_SUCCESS)
            HAL_Delay(10);
    } while(status != BLE_STATUS_SUCCESS);

    // send second 128 bytes
    do {
        status = Custom_STM_App_Update_Char_Variable_Length(CUSTOM_STM_DP, pageBuffer + 128, 128);
        if(status != BLE_STATUS_SUCCESS)
            HAL_Delay(10);
    } while(status != BLE_STATUS_SUCCESS);
}

static void packageHeader(void)
{
  BME280_ReadMeasurement_Raw(BMEraw_data);

  headerBuffer[0] = 0xAA;

  memcpy(&headerBuffer[1], BMEraw_data, sizeof(BMEraw_data));

  memcpy(&headerBuffer[9], GPSraw_data, sizeof(GPSraw_data));

  headerBuffer[24] = 0xAA;
}

static void newSectionCheck(void)
{
  if(pageCounter >= 16)
  {
    pageCounter = 0;
    sectorCounter++;
    
    if (sectorCounter >= 15625)
    {
      sectorCounter = 0;
    }  

    Flash_SectorErase(sectorCounter);
  }
}

void packageDataToMem(void)
{
  // Put header values ONCE first in buffer
  if(bufferPointer == 0)
  {
    packageHeader(); 
    memcpy(buffer, headerBuffer, sizeof(headerBuffer));
    bufferPointer += 25;
    dataCounter = 0;
  }

  if(dataCounter < 19)
  {
    BMI270_ReadMeasurement_raw(BMIraw_data);  // Read data from sensor

    memcpy(buffer + bufferPointer, BMIraw_data, sizeof(BMIraw_data)); // put data on the buffer. 

    bufferPointer += 12; 
    dataCounter++;
  }
  else
  {
    buffer[255] = pageNumber; // Just nu klarar denna att räkna upp till 255. Men vi behöver 200 000
    
    bufferFull = true;
  }
}

void sendPackageToMem(void)
{
  if (bufferFull) // Buffer is full of 256 bytes
  {
    newSectionCheck();  // Check if we are in a new section in the memory (One section is 4096 bytes)

    Flash_WritePage((sectorCounter * SECTION_SIZE) + (BUFFER_SIZE * pageCounter), buffer, BUFFER_SIZE);
    pageCounter++;
    pageNumber++;

    bufferPointer = 0;
    dataCounter = 0;

    bufferFull = false;
    
    memset(buffer, 0xFF, BUFFER_SIZE);
  }
}

void readMemSendBle(void)
{
  if (bleSector < sectorCounter || (bleSector == sectorCounter && ble_page < pageCounter))
  {
    Flash_4ByteRead((bleSector *  SECTION_SIZE) + (BUFFER_SIZE * ble_page), readbuffer, sizeof(readbuffer));

    sendPage_ble(readbuffer);

    ble_page++;

    if(ble_page >= 16)
    {
      bleSector++;
      ble_page = 0;
    }
  }
}




