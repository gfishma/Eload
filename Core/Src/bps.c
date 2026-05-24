/*
 * bps.c
 *
 *  Created on: 2020年5月21日
 *      Author: timecy
 */
#include "bsp.h"
#include "std_gpio.h"

#include "std_i2c.h"
#include "std_spi.h"
#include "Module_DVM_V2.h"


// hw
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;




// bps

uint8_t scmd_buff[1024];
gpio_class led = GPIOM_NEW(E, 0, GPIO_MODE_OUTPUT_PP);

i2c_bus_class i2c_bus_list[] =
{
		I2C_BUS_NEW(&hi2c1),
		I2C_BUS_NEW(&hi2c2),
		I2C_BUS_NEW(&hi2c3),
};
uint16_t i2c_bus_list_qty = (sizeof(i2c_bus_list)/sizeof(i2c_bus_list[0]));

spi_bus_class spi_bus_list[] =
{
		SPI_BUS_NEW(&hspi1),
		SPI_BUS_NEW(&hspi2),
		SPI_BUS_NEW(&hspi3),
};
uint16_t spi_bus_list_qty = (sizeof(spi_bus_list)/sizeof(spi_bus_list[0]));

gpio_class io_list[] =
{
		// PORT A
		GPIOM_NEW(A, 0, GPIO_MODE_INPUT),
		GPIOM_NEW(A, 3, GPIO_MODE_INPUT),
		GPIOM_NEW(A, 15, GPIO_MODE_INPUT),
		// PORT B
		GPIOM_NEW(B, 2, GPIO_MODE_INPUT),
		GPIOM_NEW(B, 8, GPIO_MODE_INPUT),
		GPIOM_NEW(B, 10, GPIO_MODE_INPUT),
		GPIOM_NEW(B, 15, GPIO_MODE_INPUT),
		// PORT C
		GPIOM_NEW(C, 0, GPIO_MODE_INPUT),
		GPIOM_NEW(C, 2, GPIO_MODE_INPUT),
		GPIOM_NEW(C, 3, GPIO_MODE_INPUT),
		GPIOM_NEW(C, 6, GPIO_MODE_INPUT),
		GPIOM_NEW(C, 7, GPIO_MODE_INPUT),
		GPIOM_NEW(C, 8, GPIO_MODE_INPUT),
		GPIOM_NEW(C, 13, GPIO_MODE_INPUT),
		// PORT D
		GPIOM_NEW(D, 0, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 1, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 3, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 4, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 7, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 8, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 9, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 10, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 11, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 12, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 13, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 14, GPIO_MODE_INPUT),
		GPIOM_NEW(D, 15, GPIO_MODE_INPUT),
		// PORT E
		GPIOM_NEW(E, 0, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 2, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 3, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 4, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 6, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 7, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 8, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 9, GPIO_MODE_OUTPUT_PP),
		GPIOM_NEW(E, 10, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 11, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 12, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 13, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 14, GPIO_MODE_INPUT),
		GPIOM_NEW(E, 15, GPIO_MODE_INPUT),
};
uint16_t io_list_qty = (sizeof(io_list)/sizeof(io_list[0]));


//w25qxx_class w25qxx_1 =
//{
//		.spi_dev = {.bus = &spi_bus_list[1], .cs = GPIOM_NEW(D, 2, GPIO_MODE_OUTPUT_PP),},
//};

//M_DVM_V2_Def DVM_V2	=
//{
//		.ADS124S0x_Chip		=
//		{
//			.SPI_Dev =
//			{
//				.bus = &spi_bus_list[0],
//				.cs = GPIOM_NEW(E, 9, GPIO_MODE_OUTPUT_PP),
//				.mode.cpol = SPI_POLARITY_LOW,
//				.mode.cpha = SPI_PHASE_2EDGE,
//			},
//			.In_Drdy = GPIOM_NEW(E, 11, GPIO_MODE_INPUT),
//			.ChipType = 0,
//			.ErrCnt = 0,
//		},
//		.DIO_Chip =
//		{
//			.i2c = { .bus = &i2c_bus_list[0], .addr_wide = i2c_8bit_mode, },
//			.id  = 1,
//		},
//		._Offset			= 0,
//		._Scale				= 1,
//};


void bsp_init()
{
//	float pVolt = 0.0;
//	float pVolt_2 = 0.0;
//	uint8_t tab[8] = {0xaa, 0xaa};
	gpio_config(&led);
//	i2c_bus_write_multi(&i2c_bus_list[0], 8, 0x70, 0x08, 1, &temp);	//
}

