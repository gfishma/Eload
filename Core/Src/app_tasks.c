/**
 ******************************************************************************
 * @file           : app_tasks.c
 * @brief          : FreeRTOS task glue — delegates to eload_* modules
 ******************************************************************************
 */

#include "app_tasks.h"
#include "eload.h"
#include "eload_ui.h"
#include "calibration.h"
#include "config.h"
#include "app_logger.h"
#include "ad5667.h"
#include "Temp.h"
#include <stdio.h>
#include <string.h>

extern char buffer_rx[255];
extern uint8_t Rec_Length;
extern volatile int g_cal_running;

void eload_cmd_dispatch(const char *buf);
void eload_ui_display(void);
void eload_ui_display_init(void);
void eload_ui_keypad(void);

/* ==================== OLED / Command Task ==================== */

static void Task_ProcessUARTCommand(void)
{
    if (Rec_Length == 0) return;

    uint16_t length = Rec_Length;
    if (length >= sizeof(buffer_rx))
        length = sizeof(buffer_rx) - 1;

    buffer_rx[length] = '\0';

    eload_cmd_dispatch(buffer_rx);

    Rec_Length = 0;
    buffer_rx[0] = '\0';
}

void Tasks_Initialize(void)
{
    /* Called by MX_FREERTOS_Init — one-time task setup */
}

static uint8_t cal_load_done = 0;

void Task_OLEDControl(void *argument)
{
    (void)argument;

    /* One-time: load calibration after FreeRTOS starts */
    if (!cal_load_done)
    {
        Calibration_Load();
        g_eload.cal_loaded = Calibration_IsLoaded();
        cal_load_done = 1;
    }

    Task_ProcessUARTCommand();
}

/* ==================== Display Task ==================== */

void Task_DisplayUpdate(void *argument)
{
    (void)argument;
    eload_ui_display();

    /* LED heartbeat */
    HAL_GPIO_TogglePin(GPIOE, LED_B_Pin);
}

/* ==================== Keypad Task ==================== */

void Task_KeypadProcess(void *argument)
{
    (void)argument;
    eload_ui_keypad();
}

/* ==================== Data Acquisition Task ==================== */

void Task_DataAcquisition(void *argument)
{
    (void)argument;

    if (g_cal_running) return;

    eload_read_measurements();
}

/* ==================== Monitor / Protection Task ==================== */

void Task_MonitorProtection(void *argument)
{
    (void)argument;

    float temp = 0.0f;
    if (Task_ReadTemperature(&temp) == HAL_OK)
    {
        g_eload.temp_c = temp;
        eload_check_protection();
    }
}

/* ==================== Helpers (kept for backward compat) ==================== */

float Task_ParseCurrentValue(const char *buffer)
{
    return -1.0f;  /* deprecated — use eload_cmd_dispatch instead */
}

HAL_StatusTypeDef Task_ApplyCurrentSetting(float current_ma)
{
    (void)current_ma;
    return HAL_ERROR;  /* deprecated */
}

HAL_StatusTypeDef Task_SampleADC(void)
{
    return HAL_OK;
}

HAL_StatusTypeDef Task_ReadCurrent(float *current)
{
    if (!current) return HAL_ERROR;
    *current = g_eload.i_meas;
    return HAL_OK;
}

HAL_StatusTypeDef Task_ReadVoltage(float *voltage)
{
    if (!voltage) return HAL_ERROR;
    *voltage = g_eload.v_meas;
    return HAL_OK;
}

HAL_StatusTypeDef Task_ReadTemperature(float *temperature)
{
    if (!temperature) return HAL_ERROR;
    *temperature = g_eload.temp_c;  /* NTC not wired — return last known */
    return HAL_OK;
}

uint8_t Task_CheckTemperatureProtection(void)
{
    return g_eload.otp_triggered ? 1 : 0;
}
