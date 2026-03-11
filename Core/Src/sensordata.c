/**
 * @file sensordata.c
 * @brief Handles packaging of sensor data and storage in flash memory.
 * @author Viktor Alexandersson & Niklas Christensen
 * @date 2025-02-27
 */

/* Includes -----------------------------------------------*/
#include "sensordata.h"

#define BMI_SAMPLES_PER_PAGE    18
#define BUFFER_SIZE             256
#define HEADER_SIZE             25
#define SECTION_SIZE            4096
#define PAGE_PER_SECTION        16

#define PAGE_IN_MEMORY          250000

static bool bufferFull = false; 

uint8_t GPSraw_data[15]; 
uint8_t BMEraw_data[8]; 
uint8_t BMIraw_data[12]; 

static uint8_t headerBuffer[HEADER_SIZE];

static uint8_t bufferPointer = 0;
static uint8_t dataCounter = 0;

static uint8_t buffer[BUFFER_SIZE];
static uint32_t addInc = 0;
static uint8_t pageCounter = 0;
static uint16_t sectorCounter = 0;

static uint8_t readBuffer[BUFFER_SIZE];

static uint32_t currentBlePage = 0;
static uint8_t  currentSubPage = 0; // 0 for first half, 1 for second half

static void packageHeader(void)
{
  BME280_ReadMeasurement_Raw(BMEraw_data);

  headerBuffer[0] = 0xAA;

  memcpy(&headerBuffer[1], BMEraw_data, sizeof(BMEraw_data));

  memcpy(&headerBuffer[9], GPSraw_data, sizeof(GPSraw_data));

  headerBuffer[24] = 0xAA;
}

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

void sendPackageToMem(void)
{
  if(!bufferFull){return;}

  newSectionCheck();  // Check if we are in a new section in the memory (One section is 4096 bytes)

  buffer[BUFFER_SIZE - 4] = (addInc >> 24) & 0xFF;
  buffer[BUFFER_SIZE - 3] = (addInc >> 16) & 0xFF;
  buffer[BUFFER_SIZE - 2] = (addInc >> 8) & 0xFF;
  buffer[BUFFER_SIZE - 1] = addInc & 0xFF;

  Flash_WritePage((sectorCounter * SECTION_SIZE) + (BUFFER_SIZE * pageCounter), buffer, BUFFER_SIZE);

  pageCounter++;
  addInc++;

  bufferPointer = 0;
  dataCounter = 0;
  bufferFull = false;
  memset(buffer, 0xFF, BUFFER_SIZE);
}

// Function to find the last written page in flash
void findStartPos(void)
{
    for (uint32_t i = 0; i < PAGE_IN_MEMORY; i++)
    {
        Flash_4ByteRead(i * BUFFER_SIZE, readBuffer, BUFFER_SIZE);

        uint32_t pageId = ((uint32_t)readBuffer[BUFFER_SIZE-4] << 24) |
                          ((uint32_t)readBuffer[BUFFER_SIZE-3] << 16) |
                          ((uint32_t)readBuffer[BUFFER_SIZE-2] << 8)  |
                          ((uint32_t)readBuffer[BUFFER_SIZE-1]);

        if (pageId == 0xFFFFFFFF)
        {
            if (i == 0)
            {
                // Flash is empty
                addInc = 0;
                pageCounter = 0;
                sectorCounter = 0;
            }
            else
            {

                Flash_4ByteRead((i - 1) * BUFFER_SIZE, readBuffer, BUFFER_SIZE);

                uint32_t pageId = ((uint32_t)readBuffer[BUFFER_SIZE-4] << 24) |
                                  ((uint32_t)readBuffer[BUFFER_SIZE-3] << 16) |
                                  ((uint32_t)readBuffer[BUFFER_SIZE-2] << 8)  |
                                  ((uint32_t)readBuffer[BUFFER_SIZE-1]);

                // Last written page is i-1
                addInc = pageId + 1; // optional: continue numbering
                pageCounter = (i % PAGE_PER_SECTION);
                sectorCounter = (i / PAGE_PER_SECTION);
            }
            return;
        }
    }

    // All pages used
    addInc = PAGE_IN_MEMORY;
    pageCounter = PAGE_IN_MEMORY % PAGE_PER_SECTION;
    sectorCounter = PAGE_IN_MEMORY / PAGE_PER_SECTION;
}

static tBleStatus sendHalfPage_ble(uint8_t *data, uint16_t length) 
{ 
    return Custom_STM_App_Update_Char_Variable_Length(CUSTOM_STM_DP, data, length);
}

void sendDataStepByStep(void) {
    if (currentBlePage < addInc) {
        
        // 1. Only read from flash if we are starting a fresh page
        if (currentSubPage == 0) {
            Flash_4ByteRead(currentBlePage * BUFFER_SIZE, readBuffer, BUFFER_SIZE);
        }
        
        // 2. Determine which half to send
        uint8_t *dataPtr = (currentSubPage == 0) ? readBuffer : (readBuffer + 128);
        
        // 3. Try to send the current piece
        if (sendHalfPage_ble(dataPtr, 128) == BLE_STATUS_SUCCESS) {
            currentSubPage++; 
            
            // 4. If both halves are done, move to the next flash page
            if (currentSubPage >= 2) {
                currentSubPage = 0;
                currentBlePage++;
            }
        }
    }
}
