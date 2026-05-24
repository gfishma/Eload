
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */



/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void pwm_channel_crr(uint16_t ch,uint16_t crr);
void pwm_channel_out(uint8_t ch,uint8_t cycle,uint16_t fre);
void pwm_channel_f(uint16_t ch,uint16_t pl);
void pwm_channel_stop(uint8_t ch);
void get_frequency(uint32_t *fyequ);
/* USER CODE BEGIN Prototypes */

#define PWM1 	1
#define PWM2 	2
#define PWM3 	3




/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

