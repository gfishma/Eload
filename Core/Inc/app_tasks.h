/**
 ******************************************************************************
 * @file           : app_tasks.h
 * @brief          : FreeRTOS task definitions and business logic separation
 * @purpose        : Defines task interfaces and decouples from FreeRTOS layer
 ******************************************************************************
 */

#ifndef __APP_TASKS_H__
#define __APP_TASKS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* ============================= Task Types =================================== */
typedef enum {
    TASK_OLED_CTRL,             /* OLED: Command parsing and current control */
    TASK_DISPLAY_UPDATE,        /* Display: LCD refresh */
    TASK_KEYPAD_PROCESS,        /* Keypad: Input processing */
    TASK_DATA_ACQUISITION,      /* Data: ADC sampling and collection */
    TASK_MONITOR_PROTECTION,    /* Monitor: Temperature protection */
} TaskType_t;

/* ============================= Task Context ================================= */
typedef struct {
    TaskType_t task_type;
    uint32_t execution_count;
    uint32_t last_execution_time;
} TaskContext_t;

/* ============================= OLED/Control Task ============================ */

/**
 * @brief OLED command parsing and current control task
 * @param argument: Task argument (unused)
 * @note  Priority: Normal, Stack: 512 bytes
 */
void Task_OLEDControl(void *argument);

/**
 * @brief Parse current value from command string
 * @param buffer: Command string (format: "set_curr(XXXX.XXmA)")
 * @return Parsed current value in mA, -1.0 if parse failed
 */
float Task_ParseCurrentValue(const char *buffer);

/**
 * @brief Apply current setting to hardware
 * @param current_ma: Current value in mA
 * @return HAL_OK on success, HAL_ERROR on failure
 */
HAL_StatusTypeDef Task_ApplyCurrentSetting(float current_ma);

/* ============================= Display Task ================================= */

/**
 * @brief Display/LCD update task
 * @param argument: Task argument (unused)
 * @note  Priority: Normal, Stack: 512 bytes
 */
void Task_DisplayUpdate(void *argument);

/**
 * @brief Update current value display on LCD
 * @param current_ma: Current value to display
 */
void Task_UpdateCurrentDisplay(float current_ma);

/**
 * @brief Update voltage display on LCD
 * @param voltage_v: Voltage value to display
 */
void Task_UpdateVoltageDisplay(float voltage_v);

/**
 * @brief Update mode display on LCD
 * @param mode: Mode string ("  +", "  -", "OFF")
 */
void Task_UpdateModeDisplay(const char *mode);

/* ============================= Keypad Task ================================== */

/**
 * @brief Keypad processing task
 * @param argument: Task argument (unused)
 * @note  Priority: Normal, Stack: 1024 bytes
 */
void Task_KeypadProcess(void *argument);

/**
 * @brief Handle SET button (mode toggle: +/-/OFF)
 * @param mode_state: Pointer to current mode state
 */
void Task_HandleKeySet(uint8_t *mode_state);

/**
 * @brief Handle UP button (increase current)
 * @param step: Step value in mA
 */
void Task_HandleKeyUp(uint16_t step);

/**
 * @brief Handle DOWN button (decrease current)
 * @param step: Step value in mA
 */
void Task_HandleKeyDown(uint16_t step);

/**
 * @brief Handle CONFIG button (change step value)
 * @param step_state: Pointer to current step state
 * @param step_value: Pointer to current step value
 */
void Task_HandleKeyConfig(uint8_t *step_state, uint16_t *step_value);

/* ============================= Data Acquisition Task ======================== */

/**
 * @brief Data acquisition task (ADC sampling)
 * @param argument: Task argument (unused)
 * @note  Priority: Normal, Stack: 1024 bytes
 */
void Task_DataAcquisition(void *argument);

/**
 * @brief Sample ADC values
 * @return HAL_OK on success
 */
HAL_StatusTypeDef Task_SampleADC(void);

/**
 * @brief Read current from ADC/sensor
 * @param current: Pointer to store current value
 * @return HAL_OK on success
 */
HAL_StatusTypeDef Task_ReadCurrent(float *current);

/**
 * @brief Read voltage from ADC/sensor
 * @param voltage: Pointer to store voltage value
 * @return HAL_OK on success
 */
HAL_StatusTypeDef Task_ReadVoltage(float *voltage);

/* ============================= Monitor/Protection Task ====================== */

/**
 * @brief Temperature monitoring and protection task
 * @param argument: Task argument (unused)
 * @note  Priority: Idle, Stack: 512 bytes
 */
void Task_MonitorProtection(void *argument);

/**
 * @brief Read temperature value
 * @param temperature: Pointer to store temperature value
 * @return HAL_OK on success
 */
HAL_StatusTypeDef Task_ReadTemperature(float *temperature);

/**
 * @brief Check temperature and apply protection if needed
 * @return True if temperature exceeds threshold
 */
uint8_t Task_CheckTemperatureProtection(void);

/* ============================= Common Task Functions ======================== */

/**
 * @brief Initialize all application tasks
 * @note  Should be called during FreeRTOS initialization
 */
void Tasks_Initialize(void);

/**
 * @brief Get task context by type
 * @param task_type: Type of task
 * @return Pointer to task context, NULL if not found
 */
TaskContext_t* Tasks_GetContext(TaskType_t task_type);

/**
 * @brief Reset task execution statistics
 */
void Tasks_ResetStats(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_TASKS_H__ */
