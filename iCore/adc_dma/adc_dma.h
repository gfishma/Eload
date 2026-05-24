#ifndef __ADC_DMA_H__
#define __ADC_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
typedef struct
{
    uint8_t i;
    uint16_t adc_ch1;
    uint16_t adc_ch2;
    uint16_t adc_buffer[20]; 

} Simple;




void adc_get(float *voltage,uint8_t ch);
void adc_get_ch(float *value,uint8_t ch);
void adc_config(void);




#ifdef __cplusplus
}
#endif

#endif 




