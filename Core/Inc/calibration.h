/**
 ******************************************************************************
 * @file           : calibration.h
 * @brief          : Eload current calibration (4 segments, 8 points), EEPROM storage
 ******************************************************************************
 */

#ifndef __CALIBRATION_H__
#define __CALIBRATION_H__

#include <stdint.h>

#define CAL_MAGIC        0xCA1B0001   /* Magic number to validate EEPROM data */
#define CAL_POINT_COUNT  16           /* 16 calibration points covering 0-2A */
#define CAL_SAMPLE_COUNT 10           /* DVM samples per point (discard hi/lo) */
#define CAL_EEPROM_ADDR  0x0000       /* EEPROM start address */
#define CAL_EEPROM_SIZE  256          /* Reserved EEPROM bytes */

/* Calibration point: (set_current_mA, measured_current_mA) */
typedef struct {
    float set_ma;       /* DAC setpoint in mA */
    float meas_ma;      /* DVM measured current in mA */
} cal_point_t;

/* EEPROM calibration table layout */
typedef struct {
    uint32_t    magic;              /* 0xCA1B0001 */
    uint8_t     point_count;        /* Number of valid calibration points */
    uint8_t     reserved[3];        /* Padding to align */
    cal_point_t points[CAL_POINT_COUNT]; /* 16 × 8 bytes */
} cal_data_t;

/**
 * @brief Run full calibration sequence (all 8 points)
 * @return 0 on success, <0 on failure
 * @note  Blocks until complete (~2-3 seconds per point)
 */
int Calibration_Run(void);

/**
 * @brief Load calibration data from EEPROM
 * @return 0 on success, -1 if no valid calibration found
 */
int Calibration_Load(void);

/**
 * @brief Save calibration data to EEPROM
 * @return 0 on success, <0 on I2C error
 */
int Calibration_Save(void);

/**
 * @brief Apply calibration correction to a target current
 * @param target_ma  Desired output current in mA
 * @return Corrected DAC setpoint in mA
 * @note  If no calibration loaded, returns target_ma unchanged
 */
float Calibration_Apply(float target_ma);

/**
 * @brief Check if calibration data is loaded
 * @return 1 if loaded, 0 if not
 */
int Calibration_IsLoaded(void);

/**
 * @brief Print calibration table to serial
 */
void Calibration_Print(void);

#endif /* __CALIBRATION_H__ */
