#ifndef __AD5667_H
#define __AD5667_H


#include "main.h"
#include "i2c.h"

#define I2C_HANDLE          hi2c1
#define DEV_ADDR_AD5667      0x1e

#define OFF          0
#define ON           1
#define without      0
#define interior     1







HAL_StatusTypeDef AD5667_IsDeviceReady(void);  //检测i2c设备是否准备好
HAL_StatusTypeDef AD5667_Writepower(uint8_t pBuffer,uint8_t uBuff);//pBuffer是否上电,ubuff 内/外部参考电压
HAL_StatusTypeDef AD5667_WriteData_without(uint16_t Data);//设置输出电压  外部参考电压
HAL_StatusTypeDef AD5667_WriteData_interior(uint16_t Data);//设置输出电压  内部参考电压
HAL_StatusTypeDef AD5667_WritData_setcurr(uint16_t Data);//设置电流  外部参考电压

#endif 
