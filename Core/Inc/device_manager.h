/**
 ******************************************************************************
 * @file           : device_manager.h
 * @brief          : Device abstraction layer for unified device management
 * @purpose        : Provides abstract interface for all hardware devices
 ******************************************************************************
 */

#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* ============================= Device IDs ================================== */
typedef enum {
    /* Power and current control */
    DEVICE_ID_DAC_AD5667,
    
    /* Temperature sensing */
    DEVICE_ID_TEMP_SENSOR,
    
    /* ADC sampling */
    DEVICE_ID_ADC_MAIN,
    DEVICE_ID_ADC_MCP3421,
    
    /* I/O devices */
    DEVICE_ID_LCD_TFTLCD,
    DEVICE_ID_EEPROM_24CXX,
    
    /* Power switch (GPIO) */
    DEVICE_ID_POWER_SWITCH,
    
    DEVICE_ID_COUNT
} DeviceID_t;

/* ============================= Generic Device Status ======================== */
typedef enum {
    DEVICE_STATUS_OK        = 0x00,
    DEVICE_STATUS_BUSY      = 0x01,
    DEVICE_STATUS_ERROR     = 0x02,
    DEVICE_STATUS_TIMEOUT   = 0x04,
    DEVICE_STATUS_NOT_READY = 0x08
} DeviceStatus_t;

/* ============================= Generic Device Interface ===================== */
typedef struct {
    DeviceID_t id;
    const char *name;
    HAL_StatusTypeDef (*init)(void);
    HAL_StatusTypeDef (*deinit)(void);
    HAL_StatusTypeDef (*check_ready)(void);
    DeviceStatus_t status;
} Device_t;

/* ============================= Public Functions ============================= */

/**
 * @brief Initialize device manager
 * @note  Should be called once at startup
 */
void DeviceManager_Init(void);

/**
 * @brief Register a device in the device manager
 * @param device: Device structure with initialization functions
 * @return HAL_OK on success
 */
HAL_StatusTypeDef DeviceManager_Register(Device_t *device);

/**
 * @brief Get device by ID
 * @param device_id: ID of the device to retrieve
 * @return Pointer to device structure, NULL if not found
 */
Device_t* DeviceManager_GetDevice(DeviceID_t device_id);

/**
 * @brief Check if device is ready
 * @param device_id: ID of the device to check
 * @return HAL_OK if ready, HAL_ERROR otherwise
 */
HAL_StatusTypeDef DeviceManager_IsReady(DeviceID_t device_id);

/**
 * @brief Get device status
 * @param device_id: ID of the device
 * @return Current device status
 */
DeviceStatus_t DeviceManager_GetStatus(DeviceID_t device_id);

/**
 * @brief Initialize all registered devices
 * @return HAL_OK if all devices initialized successfully
 */
HAL_StatusTypeDef DeviceManager_InitializeAll(void);

/**
 * @brief Deinitialize all registered devices
 * @return HAL_OK if all devices deinitialized successfully
 */
HAL_StatusTypeDef DeviceManager_DeinitializeAll(void);

/**
 * @brief Print device status for debugging
 */
void DeviceManager_PrintStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_MANAGER_H__ */
