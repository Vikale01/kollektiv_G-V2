#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

extern QSPI_HandleTypeDef hqspi;

/* Private defines *************************************************/
#define READ_IDENTIFICATION_CMD                 0x9F
#define NORMAL_READ_CMD                         0x03
#define FOUR_BYTE_READ_CMD                      0x13
#define FOUR_BYTE_MODE                          0XB7
#define PAGE_WRITE                              0x02
#define FOUR_PAGE_WRITE                         0X38

#define SECTOR_ERASE_CMD                        0x21

#define WRITE_ENABLE_CMD                        0x06
#define WRITE_DISABLE_CMD                       0x04
#define WRITE_STATUS_CMD                        0x01
#define READ_STATUSREG_CMD                      0x05
#define READ_CONFIGREG_CMD                      0x15


/* Private functions *************************************************/
void Flash_ReadJEDEC(void);
void Flash_WriteEnable(void);
void Flash_WriteDisable(void);
uint8_t Flash_ReadStatusReg(void);
void Enable_4BYTEMODE(void);
void Flash_Write4Page(uint32_t adress, uint8_t *buffer, uint32_t size);
void Flash_4ByteRead(uint32_t adress, uint8_t *buffer, uint32_t size);
void Flash_WritePage(uint32_t adress, uint8_t *buffer, uint32_t size);
void Flash_SectorErase(uint32_t adress);
uint8_t Flash_ConfigRegister(void);
void Flash_NormalRead(uint32_t adress, uint8_t *buffer, uint32_t size);

#endif