/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_includes.h"
#include "reent.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// extern uint8_t buffer_rx[255];

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* All configuration moved to config.h and config.c */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId OLEDHandle;
osThreadId DisplayHandle;
osThreadId KeyProcessHandle;
osThreadId DataAcquisitionHandle;
osThreadId MonitorHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* All business logic functions moved to app_tasks.c */
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Display_Task(void const * argument);
void KeyProcess_Task(void const * argument);
void DataAcquisition_Task(void const * argument);
void Monitor_Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* Initialize configuration and logging systems */
  Config_Init();
  Logger_Init(LOG_LEVEL_INFO);
  DeviceManager_Init();
  Tasks_Initialize();
  
  /* Initialize ADC with DMA for data acquisition */
  adc_config();
  
  LOG_INFO(LOG_MOD_TASK, "FreeRTOS initialization started");

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of OLED - 命令解析与电流控制 */
  osThreadDef(OLED, StartDefaultTask, osPriorityNormal, 1, CFG_TASK_OLED_STACK_SIZE);
  OLEDHandle = osThreadCreate(osThread(OLED), NULL);


  /* definition and creation of Display - LCD显示更新 */
  osThreadDef(Display, Display_Task, osPriorityNormal, 2, CFG_TASK_DISPLAY_STACK_SIZE);
  DisplayHandle = osThreadCreate(osThread(Display), NULL);


  /* definition and creation of KeyProcess - 按键处理 */
  osThreadDef(KeyProcess, KeyProcess_Task, osPriorityNormal, 3, CFG_TASK_KEYPROCESS_STACK_SIZE);
  KeyProcessHandle = osThreadCreate(osThread(KeyProcess), NULL);


  /* definition and creation of DataAcquisition - 数据采集 */
  osThreadDef(DataAcquisition, DataAcquisition_Task, osPriorityNormal, 0, CFG_TASK_DATAACQ_STACK_SIZE);
  DataAcquisitionHandle = osThreadCreate(osThread(DataAcquisition), NULL);


  /* definition and creation of Monitor - 温度监控与安全保护 */
  osThreadDef(Monitor, Monitor_Task, osPriorityIdle, 0, CFG_TASK_MONITOR_STACK_SIZE);
  MonitorHandle = osThreadCreate(osThread(Monitor), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
  
  LOG_INFO(LOG_MOD_TASK, "All tasks created successfully");
  
}

/* USER CODE BEGIN HelperFunctions */
/* All business logic functions moved to app_tasks.c */
/* Use Task_* functions instead */
/* USER CODE END HelperFunctions */

/* ============================= Task Implementations ========================== */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the OLED thread - 命令解析与电流控制
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  LOG_INFO(LOG_MOD_TASK, "OLED Control task started");
  
  /* Infinite loop */
  for(;;)
  {
    /* Call business logic from app_tasks.c */
    Task_OLEDControl(argument);
    
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Display_Task */
/**
* @brief Function implementing the Display thread - LCD显示更新
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Display_Task */
void Display_Task(void const * argument)
{
  /* USER CODE BEGIN Display_Task */
  LOG_INFO(LOG_MOD_TASK, "Display task started");

  /* Infinite loop */
  for(;;)
  {
    /* Call business logic from app_tasks.c */
    Task_DisplayUpdate(argument);
    
    /* Toggle LED indicator */
    HAL_GPIO_TogglePin(GPIOE, LED_B_Pin);

    osDelay(CFG_DISPLAY_REFRESH_INTERVAL);
  }
  /* USER CODE END Display_Task */
}

/* USER CODE BEGIN Header_KeyProcess_Task */
/**
* @brief Function implementing the KeyProcess thread - 按键检测与处理
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_KeyProcess_Task */
void KeyProcess_Task(void const * argument)
{
  /* USER CODE BEGIN KeyProcess_Task */
  LOG_INFO(LOG_MOD_TASK, "Keypad task started");
  
  /* Infinite loop */
  for(;;)
  {
    /* Call business logic from app_tasks.c - handles all debouncing internally */
    Task_KeypadProcess(argument);
    
    osDelay(50);
  }
  /* USER CODE END KeyProcess_Task */
}

/* USER CODE BEGIN Header_DataAcquisition_Task */
/**
* @brief Function implementing the DataAcquisition thread - 数据采集
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DataAcquisition_Task */
void DataAcquisition_Task(void const * argument)
{
  /* USER CODE BEGIN DataAcquisition_Task */
  LOG_INFO(LOG_MOD_TASK, "Data acquisition task started");
  
  /* Infinite loop */
  for(;;)
  {
    /* Call business logic from app_tasks.c - reads ADC and updates g_voltage/g_current */
    Task_DataAcquisition(argument);

    osDelay(100);
  }
  /* USER CODE END DataAcquisition_Task */
}

/* USER CODE BEGIN Header_Monitor_Task */
/**
* @brief Function implementing the Monitor thread - 温度监控与安全保护
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Monitor_Task */
void Monitor_Task(void const * argument)
{
  /* USER CODE BEGIN Monitor_Task */
  LOG_INFO(LOG_MOD_TASK, "Monitor/Protection task started");
  
  /* Infinite loop */
  for(;;)
  {
    /* Call business logic from app_tasks.c */
    // Task_MonitorProtection(argument);
    
    osDelay(CFG_TEMP_SAMPLE_INTERVAL_MS);
  }
  /* USER CODE END Monitor_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
