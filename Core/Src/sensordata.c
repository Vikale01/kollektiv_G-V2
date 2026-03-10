/**
 * @file sensordata.c
 * @brief Handles packaging of sensor data and storage in flash memory.
 * @author Viktor Alexandersson & Niklas Christensen
 * @date 2025-02-27
 */

/* Includes -----------------------------------------------*/
#include "sensordata.h"

/* Defines -----------------------------------------*/
#define BUFFER_SIZE             256
#define SECTION_SIZE            4096
#define HEADER_SIZE             25
#define BMI_SAMPLES_PER_PAGE    19

#define PAGE_PER_SECTION        16

/* Variables -----------------------------------------------*/

static uint8_t ble_sub_page = 0; // 0 = första 128 bytes, 1 = andra 128 bytes

static uint8_t bufferPointer = 0;
static uint8_t dataCounter = 0;

static uint8_t pageCounter = 0;
static uint16_t sectorCounter = 0;

static bool bufferFull = false; 

static uint8_t headerBuffer[25];

uint8_t GPSraw_data[15]; 
uint8_t BMEraw_data[8]; 
uint8_t BMIraw_data[12]; 

static uint16_t bleSector = 0;
static uint8_t ble_page = 0;

static uint8_t buffer[BUFFER_SIZE];
static uint8_t readbuffer[BUFFER_SIZE];

static uint32_t pageNumber = 0;

/* Private functions -----------------------------------------------*/

static void saveFlashIndex(void)
{
    FlashIndex_t index;
    index.pageNumber = pageNumber;
    index.pageCounter = pageCounter;
    index.sectorCounter = sectorCounter;

    Flash_SectorErase(INDEX_SECTOR); // Radera innan skrivning
    Flash_WritePage(INDEX_ADDRESS, (uint8_t*)&index, sizeof(index));
}

static bool loadFlashIndex(void)
{
    FlashIndex_t index;
    Flash_4ByteRead(INDEX_ADDRESS, (uint8_t*)&index, sizeof(index));

    if(index.pageCounter != 0xFF && index.sectorCounter != 0xFFFF)
    {
        pageNumber = index.pageNumber;
        pageCounter = index.pageCounter;
        sectorCounter = index.sectorCounter;
        return true;
    }
    return false;
}

/**
 * @brief  Non-blocking half-page sender BLE
 * @return tBleStatus status from the stack
 */
static tBleStatus sendHalfPage_ble(uint8_t *data, uint16_t length) 
{ 

    return Custom_STM_App_Update_Char_Variable_Length(CUSTOM_STM_DP, data, length);
}
/**
 * @brief  Structure the header-package.
 */
static void packageHeader(void)
{
  BME280_ReadMeasurement_Raw(BMEraw_data);

  headerBuffer[0] = 0xAA;

  memcpy(&headerBuffer[1], BMEraw_data, sizeof(BMEraw_data));

  memcpy(&headerBuffer[9], GPSraw_data, sizeof(GPSraw_data));

  headerBuffer[24] = 0xAA;
}

/**
 * @brief  Checks whether its a new section in the memory and prevents overflow
 */
static void newSectionCheck(void)
{
  if(pageCounter >= PAGE_PER_SECTION)
  {
    pageCounter = 0;
    sectorCounter++;
    
    if (sectorCounter >= 15624)
    {
      sectorCounter = 0;
    }  

    Flash_SectorErase(sectorCounter);
  }
}


/**
 * @brief  Main memory function for packaging and sending data
 */
void packageDataToMem(void)
{
  // Put header values ONCE first in buffer
  if(bufferPointer == 0)
  {
    packageHeader(); 
    memcpy(buffer, headerBuffer, sizeof(headerBuffer));
    bufferPointer += HEADER_SIZE;
    dataCounter = 0;
  }

  if(dataCounter < BMI_SAMPLES_PER_PAGE)
  {
    BMI270_ReadMeasurement_raw(BMIraw_data);  // Read data from sensor

    memcpy(buffer + bufferPointer, BMIraw_data, sizeof(BMIraw_data)); // put data on the buffer. 

    bufferPointer += 12; 
    dataCounter++;
  }
  else
  { 
    bufferFull = true;
  }
}

/**
 * @brief  Sends the structured package to memory
 */
void sendPackageToMem(void)
{
  if (bufferFull) // Buffer is full of 256 bytes
  {
    newSectionCheck();  // Check if we are in a new section in the memory (One section is 4096 bytes)

    buffer[BUFFER_SIZE - 4] = (pageNumber >> 24) & 0xFF;
    buffer[BUFFER_SIZE - 3] = (pageNumber >> 16) & 0xFF;
    buffer[BUFFER_SIZE - 2] = (pageNumber >> 8) & 0xFF;
    buffer[BUFFER_SIZE - 1] = pageNumber & 0xFF;

    Flash_WritePage((sectorCounter * SECTION_SIZE) + (BUFFER_SIZE * pageCounter), buffer, BUFFER_SIZE);

    saveFlashIndex();

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
    if ((bleSector < sectorCounter) || (bleSector == sectorCounter && ble_page < pageCounter))
    {
        if (ble_sub_page == 0) 
        {
            Flash_4ByteRead((bleSector * SECTION_SIZE) + (BUFFER_SIZE * ble_page),
                            readbuffer, sizeof(readbuffer));
        }

        // Buffer pointer to send either first or second half
        uint8_t *dataPtr = readbuffer + (ble_sub_page * 128);

        tBleStatus status = sendHalfPage_ble(dataPtr, 128);

        if (status == BLE_STATUS_SUCCESS) 
        {
            ble_sub_page++;

            // If both halves are sent, move on to the next one
            if (ble_sub_page >= 2) 
            {
                ble_sub_page = 0;
                ble_page++;

                if (ble_page >= PAGE_PER_SECTION)
                {
                    bleSector++;
                    ble_page = 0;
                }
            }
        }
    }
}

void sensordata_init(void)
{
    if(!loadFlashIndex())
    {
        pageNumber = 0;
        pageCounter = 0;
        sectorCounter = 0;
    }

    bufferPointer = 0;
    dataCounter = 0;
    bufferFull = false;
    memset(buffer, 0xFF, BUFFER_SIZE);
}

