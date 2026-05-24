/**
 ******************************************************************************
 * @file           : config.c
 * @brief          : Configuration module implementation
 ******************************************************************************
 */

#include "config.h"

/* ============================= Global Variables ============================= */
float g_voltage = 0.0f;
float g_current = 0.0f;
float g_temperature = 0.0f;
float g_current_set = 0.0f;
float g_current_disp = 0.0f;
uint32_t g_acquisition_count = 0;

/**
 * @brief Initialize configuration parameters
 */
void Config_Init(void)
{
    g_voltage = 0.0f;
    g_current = 0.0f;
    g_temperature = 0.0f;
    g_current_set = CFG_CURRENT_DEFAULT_MA;
    g_current_disp = 0.0f;
}

/**
 * @brief Get current temperature threshold
 */
float Config_GetTempThreshold(void)
{
    return CFG_TEMP_PROTECT_THRESHOLD;
}

/**
 * @brief Get maximum current limit
 */
uint16_t Config_GetMaxCurrent(void)
{
    return CFG_MAX_CURRENT_MA;
}
