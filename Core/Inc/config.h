/**
 ******************************************************************************
 * @file           : config.h
 * @brief          : Centralized configuration for the entire application
 * @purpose        : Manages all constants, thresholds, and parameters
 ******************************************************************************
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* ============================= Current Control ============================= */
#define CFG_MAX_CURRENT_MA              13000       /* Maximum current limit (mA) */
#define CFG_CURRENT_OVERFLOW_LIMIT      11001       /* Current overflow threshold */
#define CFG_CURRENT_DEFAULT_MA          0.0f        /* Default current setting */
#define CFG_CURRENT_PARSE_DIVISOR       10          /* Decimal parsing divisor */

/* ============================= Keypad Configuration ======================== */
#define CFG_STEP_VALUE_1                1           /* Step 1mA */
#define CFG_STEP_VALUE_100              100         /* Step 100mA */
#define CFG_STEP_VALUE_1000             1000        /* Step 1000mA */
#define CFG_STEP_VALUE_DEFAULT          CFG_STEP_VALUE_1

/* ============================= Temperature Protection ====================== */
#define CFG_TEMP_PROTECT_THRESHOLD      70.0f       /* Temperature protection threshold (°C) */
#define CFG_TEMP_WARN_THRESHOLD         60.0f       /* Temperature warning threshold (°C) */
#define CFG_TEMP_SAMPLE_INTERVAL_MS     1000        /* Temperature sample interval (ms) */

/* ============================= Display Configuration ======================== */
#define CFG_DISPLAY_REFRESH_INTERVAL    500         /* Display refresh interval (ms) */
#define CFG_VOLTAGE_INT_DISPLAY_X       105
#define CFG_VOLTAGE_INT_DISPLAY_Y       0
#define CFG_VOLTAGE_FRAC_DISPLAY_X      150
#define CFG_VOLTAGE_FRAC_DISPLAY_Y      0

#define CFG_CURRENT_MES_INT_DISPLAY_X   105
#define CFG_CURRENT_MES_INT_DISPLAY_Y   55
#define CFG_CURRENT_MES_FRAC_DISPLAY_X  150
#define CFG_CURRENT_MES_FRAC_DISPLAY_Y  55

#define CFG_POWER_INT_DISPLAY_X         90
#define CFG_POWER_INT_DISPLAY_Y         110
#define CFG_POWER_FRAC_DISPLAY_X        150
#define CFG_POWER_FRAC_DISPLAY_Y        110

#define CFG_CURRENT_SET_DISPLAY_X       105
#define CFG_CURRENT_SET_DISPLAY_Y       160
#define CFG_STEP_DISPLAY_X              160
#define CFG_STEP_DISPLAY_Y              200

/* ============================= RTOS Task Configuration ===================== */
#define CFG_TASK_OLED_STACK_SIZE        512
#define CFG_TASK_OLED_PRIORITY          2           /* osPriorityNormal */
#define CFG_TASK_DISPLAY_STACK_SIZE     512
#define CFG_TASK_DISPLAY_PRIORITY       2
#define CFG_TASK_KEYPROCESS_STACK_SIZE  1024
#define CFG_TASK_KEYPROCESS_PRIORITY    2
#define CFG_TASK_DATAACQ_STACK_SIZE     1024
#define CFG_TASK_DATAACQ_PRIORITY       1
#define CFG_TASK_MONITOR_STACK_SIZE     512
#define CFG_TASK_MONITOR_PRIORITY       0           /* osPriorityIdle */

/* ============================= Device Configuration ========================= */
#define CFG_ELOAD_CMD_SUCCESS           "<eload ok\r\n"
#define CFG_ELOAD_CMD_ERROR             "<eload set error\r\n"

/* ============================= Global Variables ============================= */
extern float g_voltage;
extern float g_current;
extern float g_temperature;
extern float g_current_set;
extern float g_current_disp;
extern uint32_t g_acquisition_count;

/**
 * @brief Initialize configuration parameters
 * @note  Call this function before using other modules
 */
void Config_Init(void);

/**
 * @brief Get current temperature threshold
 * @return Temperature threshold in °C
 */
float Config_GetTempThreshold(void);

/**
 * @brief Get maximum current limit
 * @return Maximum current in mA
 */
uint16_t Config_GetMaxCurrent(void);

#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H__ */
