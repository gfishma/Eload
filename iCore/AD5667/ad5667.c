#include "ad5667.h"



#define AD5667_MEMADD_SIZE I2C_MEMADD_SIZE_8BIT




HAL_StatusTypeDef AD5667_IsDeviceReady(void)//检测i2c设备是否准备好
{
      uint32_t Trials = 10;
     return HAL_I2C_IsDeviceReady(&I2C_HANDLE, DEV_ADDR_AD5667,Trials,500);
}  




HAL_StatusTypeDef AD5667_Writepower(uint8_t pBuffer,uint8_t uBuff)//是否上电及选择参考电压
{
    uint8_t power[4]={0x00,0x03,0x00,0x00};
    if(pBuffer == 1)
    {
      
      if( HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_AD5667, 0x20, AD5667_MEMADD_SIZE, &power[0], 2, 500) ==HAL_OK)
      {
        if(uBuff==1)
        {
            return(HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_AD5667, 0x38,AD5667_MEMADD_SIZE, &power[0], 2 ,500));
        }
        else if(uBuff == 0)
        {
            return(HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_AD5667, 0x38,AD5667_MEMADD_SIZE, &power[2], 2 ,500));
        }
      }

    }else if(pBuffer == 0)
    {
       return(HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_AD5667, 0x20, AD5667_MEMADD_SIZE, &power[2], 2, 500));
    }

    return 1;
}






HAL_StatusTypeDef AD5667_WriteData_without(uint16_t Data)//设置输出电压0-5000mv
{
//float Data_f=(Data/1000);
uint16_t D=Data*65535/2.5/1000;
uint8_t Data_8[2]={D>>8,D&0xff};

return (HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_AD5667, 0x18,AD5667_MEMADD_SIZE, &Data_8[0], 2 ,500));

}



HAL_StatusTypeDef AD5667_WritData_setcurr(uint16_t Data)
{
  uint16_t D=(Data*0.5+19)*65535/2.5/1000/2;
  uint8_t Data_8[2]={D>>8,D&0xff};

  return (HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_AD5667, 0x18,AD5667_MEMADD_SIZE, &Data_8[0], 2 ,500));
}




HAL_StatusTypeDef AD5667_WriteData_interior(uint16_t Data)

{
//float Data_f=Data/1000;
uint16_t D=Data*65535/5/1000;
uint8_t Data_8[2]={D&0xff,D>>8};

return (HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_AD5667, 0x18,AD5667_MEMADD_SIZE, &Data_8[0], 2 ,500));

}






