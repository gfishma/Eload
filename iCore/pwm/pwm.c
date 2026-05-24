#include "pwm.h"
#include "tim.h"
#include "cmsis_os.h"

uint32_t capture_Buf[3]={0};
uint8_t capture_Cnt=0;
uint8_t overload_Cnt=0;
uint32_t high_time;
uint32_t low_time;

double HL_time;
double LL_time;
double fre;
double duty;

/********************************************************
使用PWM铺获时在主函数里开启
 __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);//设置上升沿捕获
 HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);//开始铺获
fre:是测量出来的频率
duty:是测量出来的占空比
******************************************************/


//设置PWM输出通道ch及占空比值  zkb/pl*100%  
void pwm_channel_crr(uint16_t ch,uint16_t zkb)
{
    switch(ch)
    {
        case PWM1:
        __HAL_TIM_SET_COMPARE(&htim11,TIM_CHANNEL_1,zkb);//PWM1
        osDelay(500);break;
        case PWM2:
        __HAL_TIM_SET_COMPARE(&htim13,TIM_CHANNEL_1,zkb);//PWM2
        osDelay(500);break;
        case PWM3:
        __HAL_TIM_SET_COMPARE(&htim14,TIM_CHANNEL_1,zkb);//PWM3
        osDelay(500);break;                
        default:
        break;
    }
}

//设置PWM输出通道ch 输出频率 f=168M/(168-1+1)(pl-1+1)  
void pwm_channel_f(uint16_t ch,uint16_t pl)
{

	switch(ch)
	{    
	case PWM1:
	__HAL_TIM_SET_AUTORELOAD(&htim11,pl);//PWM1  
	osDelay(500);break;
	case PWM2:
	__HAL_TIM_SET_AUTORELOAD(&htim13,pl);//PWM2
	osDelay(500);break;
	case PWM3:
	__HAL_TIM_SET_AUTORELOAD(&htim14,pl);//PWM3
	osDelay(500);break;                
	default:
	break;
	}

}

//fre 单位是kHZ   cycle 占空比
void pwm_channel_out(uint8_t ch,uint8_t cycle,uint16_t fre)
{

	uint8_t  z_value, f_value;
	f_value=1000/fre;
	z_value=cycle*100/f_value;

	switch (ch)
	{
	case PWM1:
	HAL_TIM_PWM_Start(&htim11,TIM_CHANNEL_1);//开始通道
	__HAL_TIM_SET_AUTORELOAD(&htim11,f_value);//设置频率
	__HAL_TIM_SET_COMPARE(&htim11,TIM_CHANNEL_1,z_value); //设置占空比
	break;
	case PWM2:
	HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_1);
	__HAL_TIM_SET_AUTORELOAD(&htim13,f_value);//设置频率
	__HAL_TIM_SET_COMPARE(&htim13,TIM_CHANNEL_1,z_value); //设置占空比
	break;
	case PWM3:
	HAL_TIM_PWM_Start(&htim14,TIM_CHANNEL_1);
	__HAL_TIM_SET_AUTORELOAD(&htim14,f_value);//设置频率
	__HAL_TIM_SET_COMPARE(&htim14,TIM_CHANNEL_1,z_value); //设置占空比
	break;
	}
}


void pwm_channel_stop(uint8_t ch)
{
	
	
	switch (ch)
	{
	case PWM1:
	HAL_TIM_PWM_Stop(&htim11,TIM_CHANNEL_1);//关闭通道
	break;

	case PWM2:
	HAL_TIM_PWM_Stop(&htim13,TIM_CHANNEL_1);
	break;

	case PWM3:
	HAL_TIM_PWM_Stop(&htim14,TIM_CHANNEL_1);
	break;
	}
}



/* USER CODE BEGIN PFP */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance==htim2.Instance)
	{
	switch(capture_Cnt){
		case 0:
			capture_Buf[0]=HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_1);  //得到捕获值
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);  //设置下降沿捕获
		capture_Cnt++;
		break;
		
		case 1:
			capture_Buf[1]=HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_1);  //得到捕获值
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);  //设置上升沿捕获
		capture_Cnt++;
		break;
		
		case 2:
			capture_Buf[2]=HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_1);  //得到捕获值
		HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_1);  //停止输入捕获
		high_time=capture_Buf[1]-capture_Buf[0]+overload_Cnt*0xFFFFFFFF;  //高电平时间
		low_time=capture_Buf[2]-capture_Buf[1]+overload_Cnt*0xFFFFFFFF;  //低电平时间
		HL_time=high_time*0.001;
		LL_time=low_time*0.001;
		fre=1/(HL_time+LL_time)*1000;//fre是测量出来的频率
		duty=HL_time/(HL_time+LL_time)*100;//duty是测量出来的占空比
		capture_Cnt=0;  //清空标志位
		overload_Cnt=0;  //清空溢出标志位
				__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
		HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
		break;
		
	}
	}
}


void get_frequency(uint32_t *fyequ)
{

__HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);//设置上升沿捕获
 if(HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1)==HAL_OK)
 {
	*fyequ=fre;
 }

}


/* USER CODE END PFP */



/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

