/**
 ******************************************************************************
 * @file           : eload_cmd.c
 * @brief          : UART command parser — >set_curr, >help, >status, >eload *
 ******************************************************************************
 */
#include "eload.h"
#include "calibration.h"
#include <stdio.h>
#include <string.h>

extern volatile int g_cal_running;

/* ---- Private ---- */

static float parse_current(const char *buf)
{
    const char *p = strstr(buf, "set_curr");
    const char *e = strstr(buf, "mA");
    const char *d = strstr(buf, ".");

    if (!p || !e || e <= p) return -1.0f;

    float ip = 0.0f, dp = 0.0f;
    int div = 10;

    if (!d || d > e) d = e;

    for (const char *c = p + 9; c < d; c++)
        if (*c >= '0' && *c <= '9') ip = ip * 10.0f + (*c - '0');

    if (d < e)
        for (const char *c = d + 1; c < e; c++)
            if (*c >= '0' && *c <= '9') { dp += (*c - '0') / (float)div; div *= 10; }

    return ip + dp;
}

/* ---- Public ---- */

void eload_cmd_dispatch(const char *buf)
{
    if (strstr(buf, ">set_curr"))
    {
        float ma = parse_current(buf);
        if (ma >= 0.0f)
        {
            eload_set_current(ma);
            printf("[eload] Set %.0f mA\r\n", ma);
        }
        else
            printf("<error> invalid current format\r\n");
    }
    else if (strstr(buf, ">output(on)") || strstr(buf, ">output(ON)"))
    {
        eload_set_output(ELOAD_OUTPUT_ON);
        printf("[eload] Output ON, setpoint=%.0f mA\r\n", g_eload.setpoint_ma);
    }
    else if (strstr(buf, ">output(off)") || strstr(buf, ">output(OFF)"))
    {
        eload_set_output(ELOAD_OUTPUT_OFF);
        printf("[eload] Output OFF\r\n");
    }
    else if (strstr(buf, ">help"))
    {
        printf("\r\n");
        printf("========== Eload CC Mode ==========\r\n");
        printf(" >set_curr(XXXX.XXmA)  Set current\r\n");
        printf("   e.g. >set_curr(1000.00mA) -> 1A\r\n");
        printf("   Range: 0 ~ %d mA\r\n", ELOAD_MAX_CURRENT_MA);
        printf("\r\n");
        printf(" >output(on/off)       Toggle output\r\n");
        printf(" >help                 Show this help\r\n");
        printf(" >status               Show current status\r\n");
        printf("\r\n");
        printf("---- Calibration ----\r\n");
        printf(" >eload cal            Run (8 pts, 0~2A)\r\n");
        printf(" >eload info           Show cal table\r\n");
        printf("====================================\r\n\r\n");
    }
    else if (strstr(buf, ">status"))
    {
        printf("\r\n");
        printf("========== Status ==========\r\n");
        printf(" Output:       %s\r\n", g_eload.output ? "ON" : "OFF");
        printf(" Set Current:  %.0f mA (%.2f A)\r\n", g_eload.setpoint_ma, g_eload.setpoint_ma / 1000.0f);
        printf(" Meas Voltage: %.2f V\r\n", g_eload.v_meas);
        printf(" Meas Current: %.2f A\r\n", g_eload.i_meas);
        printf(" Meas Power:   %.2f W\r\n", g_eload.p_meas);
        printf(" Temperature:  %.1f C\r\n", g_eload.temp_c);
        printf(" Calibration:  %s\r\n", g_eload.cal_loaded ? "ACTIVE" : "NONE");
        printf("============================\r\n\r\n");
    }
    else if (strstr(buf, ">eload cal"))
    {
        printf("\r\n========== Running Calibration ==========\r\n");
        g_cal_running = 1;
        Calibration_Run();
        g_cal_running = 0;
        g_eload.cal_loaded = Calibration_IsLoaded();
    }
    else if (strstr(buf, ">eload info"))
    {
        Calibration_Print();
    }
    else
    {
        /* Unknown command — silently ignore */
    }
}
