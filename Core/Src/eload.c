/**
 ******************************************************************************
 * @file           : eload.c
 * @brief          : Electronic Load core — init, output control, protection
 ******************************************************************************
 */

#include "eload.h"
#include "eload_ui.h"
#include "ad5667.h"
#include "scmd_dvm.h"
#include "calibration.h"
#include <stdio.h>

/* ---- Global instance ---- */
eload_t g_eload;

/* ---- Public API ---- */

void eload_init(void)
{
    /* Zero DAC at boot */
    AD5667_WriteData_without(0);

    g_eload.output        = ELOAD_OUTPUT_OFF;
    g_eload.setpoint_ma   = 0.0f;
    g_eload.step_ma       = 1.0f;
    g_eload.dac_ma        = 0.0f;
    g_eload.v_meas        = 0.0f;
    g_eload.i_meas        = 0.0f;
    g_eload.p_meas        = 0.0f;
    g_eload.temp_c        = 0.0f;
    g_eload.otp_triggered = 0;
    g_eload.ovp_triggered = 0;
    g_eload.opp_triggered = 0;

    /* Load calibration from EEPROM */
    Calibration_Load();
    g_eload.cal_loaded = Calibration_IsLoaded();

    /* Init LCD layout (once, before FreeRTOS) */
    eload_ui_display_init();
}

void eload_set_output(eload_output_t on)
{
    g_eload.output = on;

    if (on == ELOAD_OUTPUT_ON)
    {
        /* Power up DAC */
        AD5667_Writepower(1, 0);

        /* Apply calibration correction */
        float dac_val = g_eload.setpoint_ma;
        if (g_eload.cal_loaded)
            dac_val = Calibration_Apply(dac_val);

        g_eload.dac_ma = dac_val;

        if (dac_val < 0.5f)
            AD5667_WriteData_without(0);
        else
            AD5667_WritData_setcurr((uint16_t)dac_val);
    }
    else
    {
        /* Zero DAC, truly off */
        AD5667_WriteData_without(0);
        g_eload.dac_ma = 0.0f;
    }
}

void eload_set_current(float target_ma)
{
    if (target_ma < 0.0f)
        target_ma = 0.0f;
    if (target_ma > ELOAD_MAX_CURRENT_MA)
        target_ma = (float)ELOAD_MAX_CURRENT_MA;

    g_eload.setpoint_ma = target_ma;

    if (g_eload.output == ELOAD_OUTPUT_ON)
        eload_set_output(ELOAD_OUTPUT_ON);  /* re-apply to DAC */
}

int eload_read_measurements(void)
{
    float volt;

    /* Read DVM CH1 for voltage */
    volt = DVM_GET(1);
    if (volt < 0.0f) return -1;
    g_eload.v_meas = volt * 3.0f;  /* voltage divider scaling */

    /* Read DVM CH2 for current */
    volt = DVM_GET(2);
    if (volt < 0.0f) return -2;
    g_eload.i_meas = volt * 4.0f;  /* shunt + gain scaling */

    eload_update_power();
    return 0;
}

void eload_update_power(void)
{
    g_eload.p_meas = g_eload.v_meas * g_eload.i_meas;
}

void eload_check_protection(void)
{
    /* Over-temperature: 85°C (IRFP260N Tj_max=175°C, safe margin) */
    if (g_eload.temp_c > 85.0f && !g_eload.otp_triggered)
    {
        g_eload.otp_triggered = 1;
        g_eload.output = ELOAD_OUTPUT_OFF;
        AD5667_WriteData_without(0);
        g_eload.dac_ma = 0.0f;
        printf("<eload OTP! temperature=%.1fC, output disabled\r\n", g_eload.temp_c);
    }

    /* Over-voltage: 60V (IRFP260N Vds=200V, safety margin 3x) */
    if (g_eload.v_meas > 60.0f && !g_eload.ovp_triggered)
    {
        g_eload.ovp_triggered = 1;
        g_eload.output = ELOAD_OUTPUT_OFF;
        AD5667_WriteData_without(0);
        g_eload.dac_ma = 0.0f;
        printf("<eload OVP! %.1fV, output off\r\n", g_eload.v_meas);
    }

    /* Over-power: 100W */
    if (g_eload.p_meas > 100.0f && !g_eload.opp_triggered)
    {
        g_eload.opp_triggered = 1;
        g_eload.output = ELOAD_OUTPUT_OFF;
        AD5667_WriteData_without(0);
        g_eload.dac_ma = 0.0f;
        printf("<eload OPP! %.1fW, output off\r\n", g_eload.p_meas);
    }
}
