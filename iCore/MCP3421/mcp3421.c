#include "mcp3421.h"
#include "i2c.h"

int MCP3421_Init(void)
{
if(HAL_I2C_IsDeviceReady(&hi2c1,MCP3421ADDR,5,500)==HAL_OK)
{
uint8_t parameter = 0x9c; //系统参数配置为18位精度，自动转换模式，无PGA增益。配置字见数据手册。
return HAL_I2C_Master_Transmit(&hi2c1,MCP3421ADDR,&parameter,1,100);
}
return 1;
}


/************************************************************************/
uint32_t MCP3421_Get_Value(void)
{
uint32_t ADC_Value;
uint8_t Adc_Temp[3]={0};
if(HAL_I2C_IsDeviceReady(&hi2c1,MCP3421ADDR,5,100)==HAL_OK)
{
HAL_I2C_Master_Receive(&hi2c1,MCP3421ADDR+1,Adc_Temp,3,100);
ADC_Value = Adc_Temp[0];
ADC_Value&=0x03;
ADC_Value<<=8;
ADC_Value+= Adc_Temp[1];
ADC_Value<<=8;
ADC_Value+= Adc_Temp[2];
}
else
{
   return  1; 
}
return  ADC_Value;
}




float Voltage_Convert(void)
{
uint32_t Voltage;
float Measure;
Voltage = MCP3421_Get_Value();
//printf(“Voltage = %d\n\r”,Voltage);
if((Voltage&0x00020000)==0x00020000) //判断最高位符号位是否为为1；为“1”则为负电压
{
Voltage&=0x0001ffff;
Voltage=(~Voltage)+1;
Voltage &=0x0001ffff;
Measure = 15.625*Voltage; //15.625为18位分辨率的精度。其他分辨率的精度见数据手册。
Measure = Measure*0.001;
//printf(“Measure Result Voltage = -%fmv\n\r”,Measure);
}
else //为“0”则正电压直接转换
{
Voltage&=0x0001ffff;
Measure = 15.625*Voltage;
Measure = Measure *0.001;
//printf(“Measure Result Voltage = %fmv\n\r”,Measure); 
}
return Measure;
}

