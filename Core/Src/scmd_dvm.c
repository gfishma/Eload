/*
 * scmd_dvm.c
 *
 *  Created on: 2020年7月22日
 *      Author: timecy
 */

#include "scmd_dvm.h"
#include "Module_DVM_V2.h"
#include "std_gpio.h"
#include "std_spi.h"
#include "std_i2c.h"
#include "app_logger.h"


extern spi_bus_class spi_bus_list[];
extern uint16_t spi_bus_list_qty;

extern i2c_bus_class i2c_bus_list[];
extern uint16_t i2c_bus_list_qty;

extern uint16_t io_list_qty;
extern gpio_class io_list[];


//DVM_V2_Init

/* 寄存器列表 */
#define reg_list_qty (sizeof(reg_list)/sizeof(reg_list[0]))
static M_DVM_V2_Def reg_list[] =
{
		{  },
		{  },
		{  },
};
// 配置常量定义
#define DVM_REG_INDEX     0
#define I2C_BUS_INDEX     2
#define SPI_BUS_INDEX     0
#define CS_IO_INDEX       34
#define DRDY_IO_INDEX     36

const uint16_t regIndex = DVM_REG_INDEX;
const uint16_t i2cIndex = I2C_BUS_INDEX;
const uint16_t spiIndex = SPI_BUS_INDEX;
const uint16_t csIndex = CS_IO_INDEX;
const uint16_t drdyIndex = DRDY_IO_INDEX;

//
//
// dvm config(dvm_0, i2c_1, spi_1, cs = io34, drdy = io36)
void DVM_config(void)
{

	// 配置
	reg_list[regIndex].DIO_Chip.i2c.bus = &i2c_bus_list[i2cIndex];
	reg_list[regIndex].ADS124S0x_Chip.SPI_Dev.bus = &spi_bus_list[spiIndex];
	reg_list[regIndex].ADS124S0x_Chip.SPI_Dev.cs = io_list[csIndex];
	reg_list[regIndex].ADS124S0x_Chip.In_Drdy = io_list[drdyIndex];
	// 配置（默认）
	reg_list[regIndex].DIO_Chip.id = 1;
	reg_list[regIndex].ADS124S0x_Chip.ChipType = 0;
	reg_list[regIndex].ADS124S0x_Chip.ErrCnt = 0;
	reg_list[regIndex]._Offset = 0;
	reg_list[regIndex]._Scale = 1;
	DVM_V2_Init(&reg_list[regIndex]);

}

float DVM_GET(unsigned char ch)
{
	float volt = 0;
	unsigned char DVM_CH;
	DVM_CH=ch;

	DVM_V2_GetVolt(&reg_list[regIndex], DVM_CH, Dvm_V2_Rang25V, Dvm_V2_Smp_Time_100MS, &volt);
	return volt;
}


