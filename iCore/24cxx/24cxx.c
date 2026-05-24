#include "24cxx.h"

#define EP24C_TIMEOUT   200
//#define EP24C_MEMADD_SIZE   I2C_MEMADD_SIZE_8BIT
#define EP24C_MEMADD_SIZE I2C_MEMADD_SIZE_16BIT



HAL_StatusTypeDef EP24C_IsDeviceReady(void){
     uint32_t Trials = 10;
     return HAL_I2C_IsDeviceReady(&I2C_HANDLE, DEV_ADDR_24CXX,Trials,EP24C_TIMEOUT);
}

HAL_StatusTypeDef EP24C_WriteOneByte(uint16_t memAdderss,uint8_t byteData){
    return HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_24CXX,memAdderss,
                            EP24C_MEMADD_SIZE,&byteData,1,EP24C_TIMEOUT);
}

HAL_StatusTypeDef EP24C_ReadOneByte(uint16_t memAdderss,uint8_t *byteData){
     return HAL_I2C_Mem_Read(&I2C_HANDLE, DEV_ADDR_24CXX+1,memAdderss,
                            EP24C_MEMADD_SIZE, byteData, 1, EP24C_TIMEOUT);

}

HAL_StatusTypeDef EP24C_ReadByte(uint16_t memAdderss,uint8_t *pBuffer,uint16_t bufferlen){
    if (bufferlen > MEM_SIZE_24CXX)
    {
        return HAL_ERROR;
    }
    return HAL_I2C_Mem_Read(&I2C_HANDLE, DEV_ADDR_24CXX+1,memAdderss,
                            EP24C_MEMADD_SIZE, pBuffer, bufferlen, EP24C_TIMEOUT);
    

}
HAL_StatusTypeDef EP24C_WriteInOnepage(uint16_t memAdderss,uint8_t *pBuffer,uint16_t bufferlen){
        if (bufferlen > MEM_SIZE_24CXX)
    {
        return HAL_ERROR;
    }
    return HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_24CXX,memAdderss,
                            EP24C_MEMADD_SIZE, pBuffer, bufferlen, EP24C_TIMEOUT);
    
}


HAL_StatusTypeDef EP24C_WriteLongData(uint16_t memAdderss,uint8_t *pBuffer,uint16_t bufferlen){

        if (bufferlen > MEM_SIZE_24CXX)
    {
        return HAL_ERROR;
    }
    HAL_StatusTypeDef result = HAL_ERROR;
    if(bufferlen <= PAGE_SIZE_24CXX)
    {
        return HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_24CXX,memAdderss,
                            EP24C_MEMADD_SIZE, pBuffer, bufferlen, EP24C_TIMEOUT);
    }

    uint8_t *pt=pBuffer;
    uint16_t pageCount = bufferlen / PAGE_SIZE_24CXX;
    for(uint16_t i = 0;i < pageCount ; i++){
      result = HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_24CXX,memAdderss,
          EP24C_MEMADD_SIZE, pt, PAGE_SIZE_24CXX, EP24C_TIMEOUT);
            pt += PAGE_SIZE_24CXX;
             memAdderss += PAGE_SIZE_24CXX;
               HAL_Delay(5);
                 if(result != HAL_OK)
                    {
                    return result;
                       }
    }

    uint8_t leftBytse = bufferlen % PAGE_SIZE_24CXX;
    if(leftBytse > 0){
        result = HAL_I2C_Mem_Write(&I2C_HANDLE, DEV_ADDR_24CXX,memAdderss,
                             EP24C_MEMADD_SIZE, pt, leftBytse, EP24C_TIMEOUT);
                          
    }
return result;

}






