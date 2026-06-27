/**
 ******************************************************************************
 * @file           : eload_ui.c
 * @brief          : LCD display update + keypad handling
 ******************************************************************************
 */
#include "eload.h"
#include "Tftlcd.h"
#include "config.h"
#include <stdio.h>

/* ---- LCD Display ---- */

void eload_ui_display(void)
{
    float v = g_eload.v_meas;
    float i = g_eload.i_meas;
    float p = g_eload.p_meas;

    uint16_t vi = (uint16_t)v;
    uint16_t ii = (uint16_t)i;
    uint16_t pi = (uint16_t)p;

    float vf = v - vi;
    float fi = i - ii;
    float pf = p - pi;

    /* Voltage (X.XXX V) */
    LCD_ShowxNum(CFG_VOLTAGE_INT_DISPLAY_X, CFG_VOLTAGE_INT_DISPLAY_Y, vi, 2, 32, 0);
    LCD_ShowxNum(CFG_VOLTAGE_FRAC_DISPLAY_X, CFG_VOLTAGE_FRAC_DISPLAY_Y, (uint32_t)(vf * 1000), 3, 32, 1);

    /* Current (X.XXX A) */
    LCD_ShowxNum(CFG_CURRENT_MES_INT_DISPLAY_X, CFG_CURRENT_MES_INT_DISPLAY_Y, ii, 2, 32, 0);
    LCD_ShowxNum(CFG_CURRENT_MES_FRAC_DISPLAY_X, CFG_CURRENT_MES_FRAC_DISPLAY_Y, (uint32_t)(fi * 1000), 3, 32, 1);

    /* Power (XXX.XXX W) */
    LCD_ShowxNum(CFG_POWER_INT_DISPLAY_X, CFG_POWER_INT_DISPLAY_Y, pi, 3, 32, 0);
    LCD_ShowxNum(CFG_POWER_FRAC_DISPLAY_X, CFG_POWER_FRAC_DISPLAY_Y, (uint32_t)(pf * 1000), 3, 32, 1);

    /* Output state + Setpoint (mA) */
    LCD_ShowString(80, CFG_CURRENT_SET_DISPLAY_Y, 52, 32, 32,
        g_eload.output ? "ON " : "OFF");
    LCD_ShowxNum(135, CFG_CURRENT_SET_DISPLAY_Y, (uint32_t)g_eload.setpoint_ma, 5, 32, 0);

    /* Step value */
    LCD_ShowxNum(CFG_STEP_DISPLAY_X, CFG_STEP_DISPLAY_Y, (uint32_t)g_eload.step_ma, 5, 32, 0);
}

void eload_ui_display_init(void)
{
    lcd_show_chinese(0, 0, 25, 32);   /* Voltage label */
    lcd_show_chinese(40, 0, 26, 32);
    LCD_ShowString(135, 0, 240, 32, 32, ".");
    LCD_ShowString(210, 0, 240, 32, 32, "V");

    lcd_show_chinese(0, 55, 25, 32);   /* Current label */
    lcd_show_chinese(40, 55, 37, 32);
    LCD_ShowString(135, 55, 240, 32, 32, ".");
    LCD_ShowString(210, 55, 240, 32, 32, "A");

    lcd_show_chinese(0, 110, 18, 32);  /* Power label */
    lcd_show_chinese(40, 110, 32, 32);
    LCD_ShowString(135, 110, 240, 32, 32, ".");
    LCD_ShowString(210, 110, 240, 32, 32, "W");

    lcd_show_chinese(0, 165, 38, 32);  /* Set */
    lcd_show_chinese(40, 165, 39, 32);  /* (mA) */

    lcd_show_chinese(0, 200, 38, 32);  /* Step label */
    lcd_show_chinese(40, 200, 39, 32);
    lcd_show_chinese(80, 200, 40, 32);
    lcd_show_chinese(120, 200, 41, 32);
}

/* ---- Keypad ---- */

static uint8_t  step_idx = 0;
static uint8_t  key_set_pressed = 0;
static uint8_t  key_up_pressed = 0;
static uint8_t  key_down_pressed = 0;
static uint8_t  key_config_pressed = 0;
static const uint16_t step_values[] = {1, 100, 1000};

void eload_ui_keypad(void)
{
    uint8_t set   = KEY_SET;
    uint8_t up    = KEY_UP1;
    uint8_t down  = KEY_DOWM;
    uint8_t conf  = KEY_CONFIG;

    /* SET: toggle output ON/OFF */
    if (set && !key_set_pressed)
    {
        eload_set_output(g_eload.output ? ELOAD_OUTPUT_OFF : ELOAD_OUTPUT_ON);
        key_set_pressed = 1;
    }
    if (!set) key_set_pressed = 0;

    /* UP: increase current */
    if (up && !key_up_pressed)
    {
        float new_ma = g_eload.setpoint_ma + g_eload.step_ma;
        if (new_ma <= ELOAD_MAX_CURRENT_MA)
            eload_set_current(new_ma);
        key_up_pressed = 1;
    }
    if (!up) key_up_pressed = 0;

    /* DOWN: decrease current */
    if (down && !key_down_pressed)
    {
        float new_ma = g_eload.setpoint_ma - g_eload.step_ma;
        if (new_ma < 0.0f) new_ma = 0.0f;
        eload_set_current(new_ma);
        key_down_pressed = 1;
    }
    if (!down) key_down_pressed = 0;

    /* CONFIG: cycle step */
    if (conf && !key_config_pressed)
    {
        step_idx = (step_idx + 1) % 3;
        g_eload.step_ma = step_values[step_idx];
        key_config_pressed = 1;
    }
    if (!conf) key_config_pressed = 0;
}
