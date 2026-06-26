/**
 ******************************************************************************
 * @file           : app_tasks.c
 * @brief          : Application task implementations
 * @purpose        : Business logic separated from FreeRTOS layer
 ******************************************************************************
 */

#include "app_tasks.h"
#include "config.h"
#include "app_logger.h"
#include <stdio.h>
#include <string.h>

/* Include device drivers */
#include "adc.h"
#include "dma.h"
#include "Tftlcd.h"
#include "adc_dma.h"
#include "pwm.h"
#include "tim.h"
#include "usart.h"
#include "ad5667.h"
#include "Temp.h"
#include "mcp3421.h"
#include "scmd_dvm.h"
#include "calibration.h"

extern volatile int g_cal_running;

extern char buffer_rx[255];
extern uint8_t Rec_Length;

/* ============================= Private Variables ============================ */
static TaskContext_t task_contexts[5] = {
    {TASK_OLED_CTRL, 0, 0},
    {TASK_DISPLAY_UPDATE, 0, 0},
    {TASK_KEYPAD_PROCESS, 0, 0},
    {TASK_DATA_ACQUISITION, 0, 0},
    {TASK_MONITOR_PROTECTION, 0, 0}
};

static uint8_t mode_state = 0;          /* 0: OFF, 1: +, 2: - */
static uint8_t step_state = 0;          /* Step value selector */
static uint16_t current_step = CFG_STEP_VALUE_DEFAULT;

/* ============================= OLED/Control Task Implementation ============== */

static void Task_ProcessUARTCommand(void)
{
    if (Rec_Length == 0) {
        return;
    }

    uint16_t length = Rec_Length;
    if (length >= sizeof(buffer_rx)) {
        length = sizeof(buffer_rx) - 1;
    }

    buffer_rx[length] = '\0';
    LOG_DEBUG(LOG_MOD_TASK, "UART command received: %s", buffer_rx);

    if (strstr(buffer_rx, "set_curr") != NULL) {
        float request_ma = Task_ParseCurrentValue(buffer_rx);
        if (request_ma >= 0.0f) {
            if (Task_ApplyCurrentSetting(request_ma) == HAL_OK) {
                LOG_INFO(LOG_MOD_TASK, "UART current command applied: %.2f mA", g_current_set);
            } else {
                LOG_ERROR(LOG_MOD_TASK, "Failed to apply UART current command");
            }
        } else {
            LOG_WARN(LOG_MOD_TASK, "Invalid UART current value");
        }
    } else if (strstr(buffer_rx, ">help") != NULL) {
        printf("\r\n");
        printf("========== Core Board Eload  ==========\r\n");
        printf(" set_curr(XXXX.XXmA)   Set target current\r\n");
        printf("   e.g. set_curr(1000.00mA) → 1A\r\n");
        printf("   e.g. set_curr(5000.00mA) → 5A\r\n");
        printf("   Range: 0 ~ %d mA\r\n", CFG_MAX_CURRENT_MA);
        printf("\r\n");
        printf(" >help                 Show this help\r\n");
        printf(" >status               Show current status\r\n");
        printf("\r\n");
        printf("---- Calibration ----\r\n");
        printf(" >eload cal            Run calibration (8pts, 0~10A)\r\n");
        printf(" >eload info           Show calibration table\r\n");
        printf("========================================\r\n\r\n");
        LOG_INFO(LOG_MOD_TASK, "Help command processed");
    } else if (strstr(buffer_rx, ">status") != NULL) {
        printf("\r\n");
        printf("========== Current Status ==========\r\n");
        if (Calibration_IsLoaded())
            printf(" Set Current:   %.0f mA -> %.0f mA (%.2f A)\r\n", g_user_target, g_current_set, g_user_target / 1000.0f);
        else
            printf(" Set Current:   %.0f mA (%.2f A)\r\n", g_current_set, g_current_set / 1000.0f);
        printf(" Meas Voltage:  %.2f V\r\n", g_voltage);
        printf(" Meas Current:  %.2f A\r\n", g_current);
        printf(" Meas Power:    %.2f W\r\n", g_voltage * g_current);
        printf(" Temperature:   %.1f C\r\n", g_temperature);
        printf(" Calibration:   %s\r\n", Calibration_IsLoaded() ? "ACTIVE" : "NONE");
        printf("====================================\r\n\r\n");
        LOG_INFO(LOG_MOD_TASK, "Status command processed");
    } else if (strstr(buffer_rx, ">eload cal") != NULL) {
        printf("\r\n========== Running Calibration ==========\r\n");
        Calibration_Run();
        LOG_INFO(LOG_MOD_TASK, "Calibration command executed");
    } else if (strstr(buffer_rx, ">eload info") != NULL) {
        Calibration_Print();
        LOG_INFO(LOG_MOD_TASK, "Calibration info command processed");
    } else {
        LOG_WARN(LOG_MOD_TASK, "Unsupported UART command");
    }

    Rec_Length = 0;
    buffer_rx[0] = '\0';
}

void Task_OLEDControl(void *argument)
{
    /* Process UART command if available */
    Task_ProcessUARTCommand();
}

float Task_ParseCurrentValue(const char *buffer)
{
    if (buffer == NULL) {
        return -1.0f;
    }

    const char *pNet = strstr(buffer, "set_curr");
    const char *pEnd = strstr(buffer, "mA");
    const char *pPoint = strstr(buffer, ".");
    
    /* Validate basic format */
    if (pNet == NULL || pEnd == NULL || pEnd <= pNet) {
        LOG_WARN(LOG_MOD_TASK, "Invalid current format");
        return -1.0f;
    }
    
    /* Use pEnd as decimal point if no actual decimal point exists */
    if (pPoint == NULL || pPoint > pEnd) {
        pPoint = pEnd;
    }
    
    float integerPart = 0.0f;
    float decimalPart = 0.0f;
    uint8_t divisor = CFG_CURRENT_PARSE_DIVISOR;
    
    /* Parse integer part */
    for (const char *p = pNet + 9; p < pPoint; p++) {
        if (*p >= '0' && *p <= '9') {
            integerPart = integerPart * 10.0f + (*p - '0');
        }
    }
    
    /* Parse decimal part */
    if (pPoint < pEnd) {
        for (const char *p = pPoint + 1; p < pEnd; p++) {
            if (*p >= '0' && *p <= '9') {
                decimalPart += (*p - '0') / (float)divisor;
                divisor *= 10;
            }
        }
    }
    
    float result = integerPart + decimalPart;
    LOG_DEBUG(LOG_MOD_TASK, "Parsed current: %.2f mA", result);
    
    return result;
}

HAL_StatusTypeDef Task_ApplyCurrentSetting(float current_ma)
{
    /* Validate current range */
    if (current_ma < 0 || current_ma > CFG_MAX_CURRENT_MA) {
        LOG_ERROR(LOG_MOD_TASK, "Current out of range: %.2f mA", current_ma);
        return HAL_ERROR;
    }

    /* Check device readiness */
    if (AD5667_IsDeviceReady() != HAL_OK) {
        LOG_ERROR(LOG_MOD_TASK, "AD5667 device not ready");
        return HAL_ERROR;
    }
    
    /* Enable power */
    if (AD5667_Writepower(1, 0) != HAL_OK) {
        LOG_ERROR(LOG_MOD_TASK, "Failed to enable power");
        return HAL_ERROR;
    }

    /* Save original target before calibration correction */
    float user_target = current_ma;

    /* Apply calibration correction if available */
    if (Calibration_IsLoaded())
    {
        float corrected = Calibration_Apply(current_ma);
        if (corrected != current_ma)
        {
            LOG_DEBUG(LOG_MOD_TASK, "Cal applied: %.1f -> %.1f mA", current_ma, corrected);
            current_ma = corrected;
        }
    }

    /* Set current value — zero uses raw DAC write to bypass +19mV offset */
    if (current_ma < 0.5f) {
        AD5667_WriteData_without(0);  /* DAC = 0V, truly zero current */
    } else {
        if (AD5667_WritData_setcurr((uint16_t)current_ma) != HAL_OK) {
            LOG_ERROR(LOG_MOD_TASK, "Failed to set current");
            return HAL_ERROR;
        }
    }

    g_current_set = current_ma;
    g_user_target = user_target;
    LOG_INFO(LOG_MOD_TASK, "Current set to: %.2f mA (target: %.2f mA)", current_ma, user_target);
    
    return HAL_OK;
}

/* ============================= Display Task Implementation =================== */

void Task_DisplayUpdate(void *argument)
{
    /* Read and display current values (single read to avoid race with DataAcquisition task) */
    float voltage = g_voltage;
    float current = g_current;
    float power = voltage * current;

    uint16_t voltage_int = (uint16_t)voltage;
    uint16_t current_int = (uint16_t)current;
    uint16_t power_int = (uint16_t)power;

    float voltage_frac = voltage - voltage_int;
    float current_frac = current - current_int;
    float power_frac = power - power_int;
    
    /* Display voltage (X.XXX V) */
    LCD_ShowxNum(CFG_VOLTAGE_INT_DISPLAY_X, CFG_VOLTAGE_INT_DISPLAY_Y, voltage_int, 2, 32, 0);
    LCD_ShowxNum(CFG_VOLTAGE_FRAC_DISPLAY_X, CFG_VOLTAGE_FRAC_DISPLAY_Y, (uint32_t)(voltage_frac * 1000), 3, 32, 1);
    
    /* Display current (X.XXX A) */
    LCD_ShowxNum(CFG_CURRENT_MES_INT_DISPLAY_X, CFG_CURRENT_MES_INT_DISPLAY_Y, current_int, 2, 32, 0);
    LCD_ShowxNum(CFG_CURRENT_MES_FRAC_DISPLAY_X, CFG_CURRENT_MES_FRAC_DISPLAY_Y, (uint32_t)(current_frac * 1000), 3, 32, 1);
    
    /* Display power (XXX.XXX W) */
    LCD_ShowxNum(CFG_POWER_INT_DISPLAY_X, CFG_POWER_INT_DISPLAY_Y, power_int, 3, 32, 0);
    LCD_ShowxNum(CFG_POWER_FRAC_DISPLAY_X, CFG_POWER_FRAC_DISPLAY_Y, (uint32_t)(power_frac * 1000), 3, 32, 1);

    /* Display current setting value (mA) under the setting label */
    LCD_ShowxNum(CFG_CURRENT_SET_DISPLAY_X, CFG_CURRENT_SET_DISPLAY_Y, (uint32_t)g_current_set, 5, 32, 0);
    
    /* Display current mode and step value separately for better alignment */
    // char mode_text[16];
    // char step_text[24];
    // const char *mode_label;
    // if (mode_state == 0) {
    //     mode_label = "OFF";
    // } else if (mode_state == 1) {
    //     mode_label = "+";
    // } else {
    //     mode_label = "-";
    // }
    // snprintf(mode_text, sizeof(mode_text), "%-4s", mode_label);
    // snprintf(step_text, sizeof(step_text), "%-8dmA", current_step);

    // LCD_ShowString(75, 165, 240, 32, 32, mode_text);
    // LCD_ShowString(CFG_STEP_DISPLAY_X, CFG_STEP_DISPLAY_Y, 80, 32, 32, step_text);
    LCD_ShowxNum(CFG_STEP_DISPLAY_X, CFG_STEP_DISPLAY_Y, (uint32_t)current_step, 5, 32, 0);

}

void Task_UpdateCurrentDisplay(float current_ma)
{
    /* Only update variable - display happens in Display_Task */
    g_current_disp = current_ma;
}

void Task_UpdateVoltageDisplay(float voltage_v)
{
    /* Only update variable - display happens in Display_Task */
    g_voltage = voltage_v;
}

void Task_UpdateModeDisplay(const char *mode)
{
    /* Only for logging/info - actual display update in Display_Task */
    if (mode != NULL) {
        LOG_DEBUG(LOG_MOD_KEY, "Mode: %s", mode);
    }
}

/* ============================= Keypad Task Implementation ==================== */

void Task_KeypadProcess(void *argument)
{
    /* Static variables for debounce tracking */
    // static uint8_t key_set_pressed = 0;
    static uint8_t key_up_pressed = 0;
    static uint8_t key_down_pressed = 0;
    static uint8_t key_config_pressed = 0;
    static uint32_t debounce_count = 0;
    
    /* Read current key states */
    // uint8_t key_set_state = KEY_SET;
    uint8_t key_up_state = KEY_UP1;
    uint8_t key_down_state = KEY_DOWM;
    uint8_t key_config_state = KEY_CONFIG;
    
    /* Handle UP button (increase current) */
    if (key_up_state && !key_up_pressed) {
        if (g_current_set + current_step <= CFG_MAX_CURRENT_MA) {
            Task_HandleKeyUp(current_step);
            LOG_DEBUG(LOG_MOD_KEY, "Current increased to: %.0f mA", g_current_set);
        } else {
            LOG_WARN(LOG_MOD_KEY, "Current limit reached");
        }
        key_up_pressed = 1;
        debounce_count++;
    }
    if (!key_up_state) {
        key_up_pressed = 0;
    }
    
    /* Handle DOWN button (decrease current) */
    if (key_down_state && !key_down_pressed) {
        if (g_current_set >= current_step) {
            Task_HandleKeyDown(current_step);
        } else {
            g_current_set = 0;
        }
        LOG_DEBUG(LOG_MOD_KEY, "Current decreased to: %.0f mA", g_current_set);
        key_down_pressed = 1;
        debounce_count++;
    }
    if (!key_down_state) {
        key_down_pressed = 0;
    }
    
    /* Handle CONFIG button (change step value) */
    if (key_config_state && !key_config_pressed) {
        step_state = (step_state + 1) % 3;  /* 0->1->2->0 */
        if (step_state == 0) {
            current_step = CFG_STEP_VALUE_1;
        } else if (step_state == 1) {
            current_step = CFG_STEP_VALUE_100;
        } else {
            current_step = CFG_STEP_VALUE_1000;
        }
        key_config_pressed = 1;
        debounce_count++;
        LOG_DEBUG(LOG_MOD_KEY, "Step changed to: %d mA (count=%lu)", current_step, debounce_count);
    }
    if (!key_config_state) {
        key_config_pressed = 0;
    }
}

void Task_HandleKeyUp(uint16_t step)
{
    if (g_current_set + step <= CFG_MAX_CURRENT_MA) {
        g_current_set += step;
        if (Task_ApplyCurrentSetting(g_current_set) == HAL_OK) {
            LOG_DEBUG(LOG_MOD_KEY, "Current increased by %d mA", step);
        }
    } else {
        LOG_WARN(LOG_MOD_KEY, "Current limit reached");
    }
}

void Task_HandleKeyDown(uint16_t step)
{
    if (g_current_set >= step) {
        g_current_set -= step;
        if (Task_ApplyCurrentSetting(g_current_set) == HAL_OK) {
            LOG_DEBUG(LOG_MOD_KEY, "Current decreased by %d mA", step);
        }
    } else {
        g_current_set = 0;
        Task_ApplyCurrentSetting(0);
    }
}

void Task_HandleKeyConfig(uint8_t *step_state, uint16_t *step_value)
{
    if (step_state == NULL || step_value == NULL) return;
    
    if (*step_state == 0) {
        *step_state = 1;
        *step_value = CFG_STEP_VALUE_100;
    }
    else if (*step_state == 1) {
        *step_state = 2;
        *step_value = CFG_STEP_VALUE_1000;
    }
    else if (*step_state == 2) {
        *step_state = 0;
        *step_value = CFG_STEP_VALUE_1;
    }
    LOG_DEBUG(LOG_MOD_KEY, "Step changed to: %d mA", *step_value);
}

/* ============================= Data Acquisition Task Implementation ========== */

void Task_DataAcquisition(void *argument)
{
    float voltage = 0.0f;
    float current = 0.0f;

    /* Skip DVM access during calibration to avoid bus conflict */
    if (g_cal_running)
        return;

    /* Sample ADC channels for voltage and current */
    Task_SampleADC();
    
    /* Read voltage from ADC channel 1 */
    if (Task_ReadVoltage(&voltage) == HAL_OK) {
        g_voltage = voltage;
        LOG_DEBUG(LOG_MOD_TASK, "Voltage: %.2f V", voltage);
    }
    
    /* Read current from ADC channel 2 */
    if (Task_ReadCurrent(&current) == HAL_OK) {
        g_current = current;
        LOG_DEBUG(LOG_MOD_TASK, "Current: %.2f A", current);
    }

    /* Update acquisition counter - display in Display_Task only */
    g_acquisition_count++;
}

HAL_StatusTypeDef Task_SampleADC(void)
{
    /* ADC or DVM sampling is handled in business logic
     * Data acquisition task still performs periodic reads */
    return HAL_OK;
}

HAL_StatusTypeDef Task_ReadCurrent(float *current)
{
    if (current == NULL) return HAL_ERROR;
    
    /* Read current from ADC channel 2 (通过adc_dma驱动读取) */
    *current=DVM_GET(2);
    
    /* Apply scaling factor: 2 / (10 * 0.05) = 4 */
    *current = (*current) * 4.0f;
    
    return HAL_OK;
}

HAL_StatusTypeDef Task_ReadVoltage(float *voltage)
{
    if (voltage == NULL) return HAL_ERROR;
    
    /* Read voltage from ADC channel 1 (通过adc_dma驱动读取) */
    *voltage=DVM_GET(1);
    
    /* Apply scaling factor: 3 */
    *voltage = (*voltage) * 3.0f;
    
    return HAL_OK;
}

/* ============================= Monitor/Protection Task Implementation ======= */

void Task_MonitorProtection(void *argument)
{
    float temperature = 0.0f;
    
    /* Read temperature */
    if (Task_ReadTemperature(&temperature) == HAL_OK) {
        g_temperature = temperature;
        
        /* Check protection threshold */
        if (Task_CheckTemperatureProtection() != 0) {
            LOG_ERROR(LOG_MOD_TEMP, "Temperature protection triggered: %.1f°C", temperature);
            /* Disable eload - set current to 0 */
            Task_ApplyCurrentSetting(0);
        }
    }
}

HAL_StatusTypeDef Task_ReadTemperature(float *temperature)
{
    if (temperature == NULL) return HAL_ERROR;
    
    /* TODO: Read from temperature sensor (Temp.h) */
    /* Example: Temp_Get(temperature); */
    *temperature = g_temperature;
    
    return HAL_OK;
}

uint8_t Task_CheckTemperatureProtection(void)
{
    if (g_temperature >= CFG_TEMP_PROTECT_THRESHOLD) {
        return 1;
    }
    if (g_temperature >= CFG_TEMP_WARN_THRESHOLD) {
        LOG_WARN(LOG_MOD_TEMP, "Temperature warning: %.1f°C", g_temperature);
    }
    return 0;
}

/* ============================= Common Functions ============================= */

void Tasks_Initialize(void)
{
    memset(task_contexts, 0, sizeof(task_contexts));
    mode_state = 0;
    step_state = 0;
    current_step = CFG_STEP_VALUE_DEFAULT;
    
    LOG_INFO(LOG_MOD_TASK, "Application tasks initialized");
}

TaskContext_t* Tasks_GetContext(TaskType_t task_type)
{
    if (task_type < 5) {
        return &task_contexts[task_type];
    }
    return NULL;
}

void Tasks_ResetStats(void)
{
    for (int i = 0; i < 5; i++) {
        task_contexts[i].execution_count = 0;
        task_contexts[i].last_execution_time = 0;
    }
    LOG_INFO(LOG_MOD_TASK, "Task statistics reset");
}
