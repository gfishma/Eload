/**
 ******************************************************************************
 * @file           : calibration.c
 * @brief          : Eload 4-segment calibration, stores to CAT24C08 EEPROM
 *
 * Calibration points (mA): 50, 100, 200, 500, 1000, 3000, 5000, 10000
 * Segments: 0~100mA, 100mA~1A, 1A~5A, 5A~10A
 ******************************************************************************
 */

#include "calibration.h"
#include "24cxx.h"
#include "ad5667.h"
#include "scmd_dvm.h"
#include "app_tasks.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* External globals needed for current control */
extern float g_current_set;

/* Calibration data in RAM */
static cal_data_t cal_data;
static int cal_loaded = 0;

/* Predefined calibration setpoints (mA) */
static const float cal_setpoints[CAL_POINT_COUNT] = {
    50.0f, 100.0f, 200.0f, 500.0f,
    800.0f, 1200.0f, 1600.0f, 2000.0f
};

/* ---- Internal helpers ---- */

static int float_cmp(const void *a, const void *b)
{
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

/* Read DVM current (mA) via DVM CH2.
 * Formula from app_tasks.c: I = DVM_V * 4 (Amps), returns mA. */
static float read_dvm_current_ma(void)
{
    float volt = DVM_GET(2);
    if (volt < 0.0f)
        return -1.0f;

    float current_a = volt * 4.0f;    /* Amps */
    return current_a * 1000.0f;       /* mA */
}

/* Set DAC current directly (bypass slew) and wait for settling */
static int set_current_raw(float ma)
{
    if (ma < 0.0f || ma > (float)CFG_MAX_CURRENT_MA)
        return -1;

    /* Power up DAC if needed */
    AD5667_Writepower(1, 0);

    /* Write directly, no slew */
    AD5667_WritData_setcurr((uint16_t)ma);

    g_current_set    = ma;

    /* Wait for output to settle (~500ms) */
    HAL_Delay(500);

    return 0;
}

/* Measure one calibration point: set current, sample N times, return average */
static int measure_point(float setpoint_ma, float *pMeasured_ma)
{
    float samples[CAL_SAMPLE_COUNT];
    int i, valid;

    if (set_current_raw(setpoint_ma) != 0)
        return -1;

    /* Acquire samples */
    valid = 0;
    for (i = 0; i < CAL_SAMPLE_COUNT; i++)
    {
        float val = read_dvm_current_ma();
        if (val < 0.0f)
            continue;
        samples[valid++] = val;
        HAL_Delay(100);
    }

    if (valid < 3)
        return -2;  /* not enough valid readings */

    /* Sort and discard hi/lo */
    qsort(samples, valid, sizeof(float), float_cmp);

    /* Average of middle samples (skip 1st and last) */
    float sum = 0.0f;
    int mid_count = valid - 2;
    for (i = 1; i < valid - 1; i++)
        sum += samples[i];

    *pMeasured_ma = sum / (float)mid_count;
    return 0;
}

/* ---- Public API ---- */

/* Flag to pause DataAcquisition during calibration */
volatile int g_cal_running = 0;

int Calibration_Run(void)
{
    int i, ret;
    cal_point_t temp_points[CAL_POINT_COUNT];
    int ok_count = 0;

    g_cal_running = 1;  /* pause DataAcquisition to avoid bus conflict */
    printf("\r\n========== Eload Calibration ==========\r\n");

    /* Check EEPROM ready */
    if (EP24C_IsDeviceReady() != HAL_OK)
    {
        printf("<cal error> EEPROM device not ready\r\n");
        return -1;
    }

    /* Check DVM ready */
    if (AD5667_IsDeviceReady() != HAL_OK)
    {
        printf("<cal error> DAC device not ready\r\n");
        return -2;
    }

    /* Measure each calibration point */
    for (i = 0; i < CAL_POINT_COUNT; i++)
    {
        float measured;

        printf("  Point %d: set %.0f mA ... ", i + 1, (double)cal_setpoints[i]);

        ret = measure_point(cal_setpoints[i], &measured);
        if (ret != 0)
        {
            printf("FAIL (code=%d)\r\n", ret);
            continue;
        }

        temp_points[i].set_ma  = cal_setpoints[i];
        temp_points[i].meas_ma = measured;
        ok_count++;

        printf("measured %.1f mA  (error=%.1f%%)\r\n",
            (double)measured,
            (double)((measured - cal_setpoints[i]) / cal_setpoints[i] * 100.0f));
    }

    if (ok_count < 2)
    {
        printf("<cal error> Not enough valid points (%d)\r\n", ok_count);
        return -3;
    }

    /* Store results */
    memset(&cal_data, 0, sizeof(cal_data));
    cal_data.magic       = CAL_MAGIC;
    cal_data.point_count = (uint8_t)ok_count;
    for (i = 0; i < ok_count; i++)
        cal_data.points[i] = temp_points[i];

    /* Save to EEPROM */
    ret = Calibration_Save();
    if (ret != 0)
    {
        printf("<cal error> EEPROM write failed\r\n");
        return -4;
    }

    cal_loaded = 1;
    /* Turn off current after calibration */
    AD5667_WriteData_without(0);
    g_current_set = 0;

    printf("<cal ok> %d points saved to EEPROM\r\n", ok_count);
    printf("========================================\r\n\r\n");

    g_cal_running = 0;
    return 0;
}

int Calibration_Load(void)
{
    uint8_t buf[sizeof(cal_data_t)];
    HAL_StatusTypeDef ret;

    if (EP24C_IsDeviceReady() != HAL_OK)
    {
        cal_loaded = 0;
        return -1;
    }

    /* Read full calibration block from EEPROM */
    ret = EP24C_ReadByte(CAL_EEPROM_ADDR, buf, sizeof(cal_data_t));
    if (ret != HAL_OK)
    {
        cal_loaded = 0;
        return -1;
    }

    memcpy(&cal_data, buf, sizeof(cal_data_t));

    /* Validate magic number */
    if (cal_data.magic != CAL_MAGIC || cal_data.point_count < 2)
    {
        memset(&cal_data, 0, sizeof(cal_data));
        cal_loaded = 0;
        return -1;
    }

    cal_loaded = 1;
    return 0;
}

int Calibration_Save(void)
{
    uint8_t buf[sizeof(cal_data_t)];

    if (EP24C_IsDeviceReady() != HAL_OK)
        return -1;

    memcpy(buf, &cal_data, sizeof(cal_data_t));
    return EP24C_WriteLongData(CAL_EEPROM_ADDR, buf, sizeof(cal_data_t));
}

float Calibration_Apply(float target_ma)
{
    int i;
    float x0, x1, y0, y1;

    if (!cal_loaded || cal_data.point_count < 2)
        return target_ma;  /* no calibration → passthrough */

    /* Find the segment containing target_ma */
    for (i = 0; i < (int)cal_data.point_count - 1; i++)
    {
        if (target_ma <= cal_data.points[i + 1].set_ma)
        {
            /* Linear interpolation between points i and i+1 */
            x0 = cal_data.points[i].set_ma;
            x1 = cal_data.points[i + 1].set_ma;
            y0 = cal_data.points[i].meas_ma;
            y1 = cal_data.points[i + 1].meas_ma;

            /* Error at this target: delta = meas - set */
            float t  = (target_ma - x0) / (x1 - x0);
            float err = y0 + t * (y1 - y0) - target_ma;

            /* Correct: setpoint_compensated = target - error */
            return target_ma - err;
        }
    }

    /* Target above highest calibration point — use last segment slope */
    {
        int n = cal_data.point_count;
        x0 = cal_data.points[n - 2].set_ma;
        x1 = cal_data.points[n - 1].set_ma;
        y0 = cal_data.points[n - 2].meas_ma;
        y1 = cal_data.points[n - 1].meas_ma;

        float t  = (target_ma - x0) / (x1 - x0);
        float err = y0 + t * (y1 - y0) - target_ma;
        return target_ma - err;
    }
}

int Calibration_IsLoaded(void)
{
    return cal_loaded;
}

void Calibration_Print(void)
{
    int i;

    if (!cal_loaded)
    {
        printf("<eload cal> no calibration data\r\n");
        return;
    }

    printf("\r\n===== Calibration Table (%d points) =====\r\n", cal_data.point_count);
    printf("  Point |  Set(mA)  |  Meas(mA)  |  Error(%%)\r\n");
    printf("  ------|-----------|------------|----------\r\n");
    for (i = 0; i < (int)cal_data.point_count; i++)
    {
        float err_pct = (cal_data.points[i].meas_ma - cal_data.points[i].set_ma)
                        / cal_data.points[i].set_ma * 100.0f;
        printf("  %5d | %9.1f | %10.1f | %+8.2f\r\n",
            i + 1,
            (double)cal_data.points[i].set_ma,
            (double)cal_data.points[i].meas_ma,
            (double)err_pct);
    }
    printf("==========================================\r\n\r\n");
}
