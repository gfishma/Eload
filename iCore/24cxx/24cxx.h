#ifndef __24CXX_H
#define __24CXX_H


#include "main.h"
#include "i2c.h"

#define I2C_HANDLE          hi2c1
#define DEV_ADDR_24CXX      0x00A4

#define PAGE_SIZE_24CXX     0x0008
#define MEM_SIZE_24CXX      (uint16_t)256


HAL_StatusTypeDef EP24C_IsDeviceReady(void);  //检测EEPROM设备是否准备好
HAL_StatusTypeDef EP24C_WriteOneByte(uint16_t memAdderss,uint8_t byteData);//任意地址写入一个字节
HAL_StatusTypeDef EP24C_ReadOneByte(uint16_t memAdderss,uint8_t *byteData);//任意地址读出一个字节
HAL_StatusTypeDef EP24C_ReadByte(uint16_t memAdderss,uint8_t *pBuffer,uint16_t bufferlen);//任意地址，读出任意长度的数据
HAL_StatusTypeDef EP24C_WriteInOnepage(uint16_t memAdderss,uint8_t *pBuffer,uint16_t bufferlen);//限定在一个页内，写入连续的数据
HAL_StatusTypeDef EP24C_WriteLongData(uint16_t memAdderss,uint8_t *pBuffer,uint16_t bufferlen);//写入任意长的数据，可以超出8字节，但是必须页首地址开始




#endif 
