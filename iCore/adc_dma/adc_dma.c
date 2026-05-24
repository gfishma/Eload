#include "adc_dma.h"
#include "adc.h"
#include "dma.h"
#include "string.h"

Simple adc;
/*
void adc_get(float *voltage,uint8_t ch)
{

 //   HAL_ADC_ErrorCallback(&hadc1);
    HAL_ADC_Start_DMA(&hadc1,(uint32_t *)W.adc_buffer,20);
    for(W.i=0,W.adc_ch1=0,W.adc_ch2=0;W.i<20;)
    {

       W.adc_ch1+=W.adc_buffer[W.i++];
       W.adc_ch2+=W.adc_buffer[W.i++];

    }
    if(ch==1)
    {      
                             
    *voltage=W.adc_ch1*3.3f/(4096*10);
    //HAL_ADC_Stop_IT(&hadc1);
//     memset(W.adc_buffer,0x00,sizeof(&W.adc_buffer));//清除数组，全部置0
    }
     else if(ch==2)
     {          

      *voltage=W.adc_ch2*3.3f/(4096*10);
   //    HAL_ADC_Stop_IT(&hadc1);
//       memset(W.adc_buffer,0x00,sizeof(&W.adc_buffer));//清除数组，全部置0
     }

}
*/

void adc_config(void)
{
    HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adc.adc_buffer,20);
}








