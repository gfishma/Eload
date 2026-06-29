/**
 ******************************************************************************
 * @file           : eload.h
 * @brief          : Electronic Load core — state machine, globals, protection
 ******************************************************************************
 */
#ifndef __ELOAD_H__
#define __ELOAD_H__

#include <stdint.h>

/* ---- Constants ---- */
#define ELOAD_MAX_CURRENT_MA    10000    /* 10A */

/* ---- Types ---- */
typedef enum {
    ELOAD_OUTPUT_OFF = 0,
    ELOAD_OUTPUT_ON  = 1,
} eload_output_t;

typedef struct {
    /* User-facing state */
    eload_output_t output;          /* Output ON/OFF */
    float          setpoint_ma;     /* User target (mA) */
    float          step_ma;         /* Keypad step size (mA) */

    /* Hardware state */
    float          dac_ma;          /* Actual DAC value written (mA, after cal) */
    float          v_meas;          /* Measured voltage (V) */
    float          i_meas;          /* Measured current (A) */
    float          p_meas;          /* Measured power (W) */
    float          temp_c;          /* Temperature (°C) */

    /* Calibration */
    uint8_t        cal_loaded;      /* 1 if valid cal loaded from EEPROM */

    /* Protection */
    uint8_t        otp_triggered;   /* Over-temperature protection */
    uint8_t        ovp_triggered;   /* Over-voltage protection */
    uint8_t        opp_triggered;   /* Over-power protection */
} eload_t;

/* ---- Global instance ---- */
extern eload_t g_eload;

/* ---- Core API ---- */

/** Initialize eload state, load calibration, zero DAC */
void eload_init(void);

/** Set output ON/OFF. ON writes DAC to setpoint, OFF zeros DAC. */
void eload_set_output(eload_output_t on);

/** Set target current in mA. Applies calibration if loaded. */
void eload_set_current(float target_ma);

/** Read back measurements from DVM. Returns 0 on success. */
int eload_read_measurements(void);

/** Check protection thresholds and shut down if exceeded. */
void eload_check_protection(void);

/** Recalculate power from measured V and I */
void eload_update_power(void);

#endif /* __ELOAD_H__ */
