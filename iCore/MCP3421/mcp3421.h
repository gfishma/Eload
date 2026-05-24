#ifndef _MCP3421_H
#define _MCP3421_H

#include "main.h"

#define MCP3421ADDR 0xD6 //MCP3421 地址

int MCP3421_Init(void); //MCP3421初始化

uint32_t MCP3421_Get_Value(void); //采集ADC数字值

float Voltage_Convert(void); //将数字量转换为电压值支持正负显示

#endif
