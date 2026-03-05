#include "flash.h"


//   uint32_t Instruction;        /* Specifies the Instruction to be sent
//                                   This parameter can be a value (8-bit) between 0x00 and 0xFF */
//   uint32_t Address;            /* Specifies the Address to be sent (Size from 1 to 4 bytes according AddressSize)
//                                   This parameter can be a value (32-bits) between 0x0 and 0xFFFFFFFF */
//   uint32_t AlternateBytes;     /* Specifies the Alternate Bytes to be sent (Size from 1 to 4 bytes according AlternateBytesSize)
//                                   This parameter can be a value (32-bits) between 0x0 and 0xFFFFFFFF */
//   uint32_t AddressSize;        /* Specifies the Address Size
//                                   This parameter can be a value of @ref QSPI_AddressSize */
//   uint32_t AlternateBytesSize; /* Specifies the Alternate Bytes Size
//                                   This parameter can be a value of @ref QSPI_AlternateBytesSize */
//   uint32_t DummyCycles;        /* Specifies the Number of Dummy Cycles.
//                                   This parameter can be a number between 0 and 31 */
//   uint32_t InstructionMode;    /* Specifies the Instruction Mode
//                                   This parameter can be a value of @ref QSPI_InstructionMode */
//   uint32_t AddressMode;        /* Specifies the Address Mode
//                                   This parameter can be a value of @ref QSPI_AddressMode */
//   uint32_t AlternateByteMode;  /* Specifies the Alternate Bytes Mode
//                                   This parameter can be a value of @ref QSPI_AlternateBytesMode */
//   uint32_t DataMode;           /* Specifies the Data Mode (used for dummy cycles and data phases)
//                                   This parameter can be a value of @ref QSPI_DataMode */
//   uint32_t NbData;             /* Specifies the number of data to transfer. (This is the number of bytes)
//                                   This parameter can be any value between 0 and 0xFFFFFFFF (0 means undefined length
//                                   until end of memory)*/
//   uint32_t DdrMode;            /* Specifies the double data rate mode for address, alternate byte and data phase
//                                   This parameter can be a value of @ref QSPI_DdrMode */
//   uint32_t SIOOMode;           /* Specifies the send instruction only once mode
//                                   This parameter can be a value of @ref QSPI_SIOOMode */

void Flash_WaitWhileBusy(void)
{
    while (Flash_ReadStatusReg() & 0x01);
}

void Flash_ReadJEDEC(void)
{
    QSPI_CommandTypeDef cmd = {0};

    uint8_t jedec_id[3];

    // Config command
    cmd.Instruction     = READ_IDENTIFICATION_CMD;                   
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE; 
    cmd.AddressMode     = QSPI_ADDRESS_NONE;       
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode        = QSPI_DATA_1_LINE;       
    cmd.DummyCycles     = 0;                       
    cmd.NbData          = 3;                       
    cmd.DdrMode         = QSPI_DDR_MODE_DISABLE;
    cmd.SIOOMode        = QSPI_SIOO_INST_EVERY_CMD;

    if(HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    if(HAL_QSPI_Receive(&hqspi, jedec_id, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
}

void Flash_WriteEnable(void)
{
    QSPI_CommandTypeDef cmd = {0};

    cmd.Instruction         = WRITE_ENABLE_CMD;
    cmd.Address             = 0;
    cmd.AlternateBytes      = 0;
    cmd.AddressSize         = 0;
    cmd.AlternateBytesSize  = 0;
    cmd.DummyCycles         = 0;
    cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode         = 0;
    cmd.AlternateByteMode   = 0;
    cmd.DataMode            = 0;
    cmd.NbData              = 0;
    cmd.DdrMode             = 0;
    cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
}

void Flash_WritePage(uint32_t adress, uint8_t *buffer, uint32_t size)
{
    QSPI_CommandTypeDef cmd = {0};

    if (size > 256) { size = 256; }

    Flash_WriteEnable();

    cmd.Instruction         = PAGE_WRITE;
    cmd.Address             = adress;
    cmd.AlternateBytes      = 0;
    cmd.AddressSize         = QSPI_ADDRESS_32_BITS;
    cmd.AlternateBytesSize  = 0;
    cmd.DummyCycles         = 0;
    cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
    cmd.AlternateByteMode   = 0;
    cmd.DataMode            = QSPI_DATA_1_LINE;
    cmd.NbData              = size;
    cmd.DdrMode             = 0;
    cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_QSPI_Transmit(&hqspi, buffer, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    Flash_WaitWhileBusy();
    Flash_WriteDisable();
}

void Flash_Write4Page(uint32_t section, uint8_t *buffer, uint32_t size)
{
    uint32_t adress = section * 256;

    QSPI_CommandTypeDef cmd = {0};

    if (size > 256) { size = 256; }

    Flash_WriteEnable();

    cmd.Instruction         = FOUR_PAGE_WRITE;
    cmd.Address             = adress;
    cmd.AlternateBytes      = 0;
    cmd.AddressSize         = QSPI_ADDRESS_32_BITS;
    cmd.AlternateBytesSize  = 0;
    cmd.DummyCycles         = 0;
    cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
    cmd.AlternateByteMode   = 0;
    cmd.DataMode            = QSPI_DATA_4_LINES;
    cmd.NbData              = size;
    cmd.DdrMode             = 0;
    cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_QSPI_Transmit(&hqspi, buffer, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    Flash_WaitWhileBusy();
    Flash_WriteDisable();
}

void Flash_WriteDisable(void)
{
    QSPI_CommandTypeDef cmd = {0};


    cmd.Instruction         = WRITE_DISABLE_CMD;
    cmd.Address             = 0;
    cmd.AlternateBytes      = 0;
    cmd.AddressSize         = 0;
    cmd.AlternateBytesSize  = 0;
    cmd.DummyCycles         = 0;
    cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode         = 0;
    cmd.AlternateByteMode   = 0;
    cmd.DataMode            = 0;
    cmd.NbData              = 0;
    cmd.DdrMode             = 0;
    cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
}

uint8_t Flash_ReadStatusReg(void)
{
    QSPI_CommandTypeDef cmd = {0};

    uint8_t status = 0;

    cmd.Instruction         = READ_STATUSREG_CMD;
    cmd.Address             = 0;
    cmd.AlternateBytes      = 0;
    cmd.AddressSize         = 0;
    cmd.AlternateBytesSize  = 0;
    cmd.DummyCycles         = 0;
    cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode         = 0;
    cmd.AlternateByteMode   = 0;
    cmd.DataMode            = QSPI_DATA_1_LINE;
    cmd.NbData              = 1;
    cmd.DdrMode             = 0;
    cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_QSPI_Receive(&hqspi, &status, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    return status;
}

uint8_t Flash_ConfigRegister(void)
{
    QSPI_CommandTypeDef cmd = {0};

    uint8_t status = 0;

    cmd.Instruction         = READ_CONFIGREG_CMD;
    cmd.Address             = 0;
    cmd.AlternateBytes      = 0;
    cmd.AddressSize         = 0;
    cmd.AlternateBytesSize  = 0;
    cmd.DummyCycles         = 0;
    cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode         = 0;
    cmd.AlternateByteMode   = 0;
    cmd.DataMode            = QSPI_DATA_1_LINE;
    cmd.NbData              = 1;
    cmd.DdrMode             = 0;
    cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_QSPI_Receive(&hqspi, &status, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    return status;
}

void Enable_4BYTEMODE(void)
{
    QSPI_CommandTypeDef cmd = {0};

    cmd.Instruction         = FOUR_BYTE_MODE;
    cmd.Address             = 0;
    cmd.AlternateBytes      = 0;
    cmd.AddressSize         = 0;
    cmd.AlternateBytesSize  = 0;
    cmd.DummyCycles         = 0;
    cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode         = 0;
    cmd.AlternateByteMode   = 0;
    cmd.DataMode            = 0;
    cmd.NbData              = 0;
    cmd.DdrMode             = 0;
    cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }    
}

void Flash_NormalRead(uint32_t adress, uint8_t *buffer, uint32_t size)
{
    QSPI_CommandTypeDef cmd = {0};

    cmd.Instruction         = NORMAL_READ_CMD;
    cmd.Address             = adress;
    cmd.AlternateBytes      = 0;
    cmd.AddressSize         = 0;
    cmd.AlternateBytesSize  = 0;
    cmd.DummyCycles         = 0;
    cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
    cmd.AlternateByteMode   = 0;
    cmd.DataMode            = QSPI_DATA_1_LINE;
    cmd.NbData              = size;
    cmd.DdrMode             = 0;
    cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_QSPI_Receive(&hqspi, buffer, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
}

void Flash_4ByteRead(uint32_t adress, uint8_t *buffer, uint32_t size)
{
    QSPI_CommandTypeDef cmd = {0};

    cmd.Instruction         = FOUR_BYTE_READ_CMD;
    cmd.Address             = adress;
    cmd.AlternateBytes      = 0;
    cmd.AddressSize         = QSPI_ADDRESS_32_BITS;
    cmd.AlternateBytesSize  = 0;
    cmd.DummyCycles         = 0;
    cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
    cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode            = QSPI_DATA_1_LINE;
    cmd.NbData              = size;
    cmd.DdrMode             = 0;
    cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_QSPI_Receive(&hqspi, buffer, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    Flash_WaitWhileBusy();
}

void Flash_SectorErase(uint32_t sector)
{
    Flash_WriteEnable();

    uint32_t adress = sector * 4096;

    QSPI_CommandTypeDef cmd = {0};

    cmd.Instruction         = SECTOR_ERASE_CMD;
    cmd.Address             = adress;
    cmd.AlternateBytes      = 0;
    cmd.AddressSize         = QSPI_ADDRESS_32_BITS;
    cmd.AlternateBytesSize  = 0;
    cmd.DummyCycles         = 0;
    cmd.InstructionMode     = QSPI_INSTRUCTION_1_LINE;
    cmd.AddressMode         = QSPI_ADDRESS_1_LINE;
    cmd.AlternateByteMode   = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode            = QSPI_DATA_NONE;
    cmd.NbData              = 0;
    cmd.DdrMode             = 0;
    cmd.SIOOMode            = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    Flash_WaitWhileBusy();
    Flash_WriteDisable();
}