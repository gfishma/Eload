/**
 ******************************************************************************
 * @file           : device_manager.c
 * @brief          : Device manager implementation
 ******************************************************************************
 */

#include "device_manager.h"
#include "app_logger.h"
#include <string.h>

/* ============================= Private Definitions =========================== */
#define MAX_DEVICES 16

/* ============================= Private Variables ============================ */
static Device_t *devices[MAX_DEVICES];
static uint8_t device_count = 0;

/* ============================= Public Functions ============================= */

/**
 * @brief Initialize device manager
 */
void DeviceManager_Init(void)
{
    memset(devices, 0, sizeof(devices));
    device_count = 0;
    LOG_INFO(LOG_MOD_DEVICE, "Device Manager initialized");
}

/**
 * @brief Register a device in the device manager
 */
HAL_StatusTypeDef DeviceManager_Register(Device_t *device)
{
    if (device == NULL) {
        LOG_ERROR(LOG_MOD_DEVICE, "Cannot register NULL device");
        return HAL_ERROR;
    }

    if (device_count >= MAX_DEVICES) {
        LOG_ERROR(LOG_MOD_DEVICE, "Device list is full");
        return HAL_ERROR;
    }

    devices[device_count] = device;
    device->status = DEVICE_STATUS_NOT_READY;
    device_count++;

    LOG_INFO(LOG_MOD_DEVICE, "Device registered: %s (ID=%d)", 
             device->name, device->id);
    return HAL_OK;
}

/**
 * @brief Get device by ID
 */
Device_t* DeviceManager_GetDevice(DeviceID_t device_id)
{
    for (uint8_t i = 0; i < device_count; i++) {
        if (devices[i] != NULL && devices[i]->id == device_id) {
            return devices[i];
        }
    }
    return NULL;
}

/**
 * @brief Check if device is ready
 */
HAL_StatusTypeDef DeviceManager_IsReady(DeviceID_t device_id)
{
    Device_t *device = DeviceManager_GetDevice(device_id);
    
    if (device == NULL) {
        LOG_WARN(LOG_MOD_DEVICE, "Device not found: %d", device_id);
        return HAL_ERROR;
    }

    if (device->check_ready != NULL) {
        return device->check_ready();
    }

    return (device->status == DEVICE_STATUS_OK) ? HAL_OK : HAL_ERROR;
}

/**
 * @brief Get device status
 */
DeviceStatus_t DeviceManager_GetStatus(DeviceID_t device_id)
{
    Device_t *device = DeviceManager_GetDevice(device_id);
    
    if (device == NULL) {
        return DEVICE_STATUS_NOT_READY;
    }

    return device->status;
}

/**
 * @brief Initialize all registered devices
 */
HAL_StatusTypeDef DeviceManager_InitializeAll(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    for (uint8_t i = 0; i < device_count; i++) {
        if (devices[i] != NULL && devices[i]->init != NULL) {
            LOG_DEBUG(LOG_MOD_DEVICE, "Initializing device: %s", devices[i]->name);
            
            if (devices[i]->init() == HAL_OK) {
                devices[i]->status = DEVICE_STATUS_OK;
                LOG_INFO(LOG_MOD_DEVICE, "Device initialized: %s", devices[i]->name);
            } else {
                devices[i]->status = DEVICE_STATUS_ERROR;
                LOG_ERROR(LOG_MOD_DEVICE, "Failed to initialize device: %s", 
                         devices[i]->name);
                status = HAL_ERROR;
            }
        }
    }

    return status;
}

/**
 * @brief Deinitialize all registered devices
 */
HAL_StatusTypeDef DeviceManager_DeinitializeAll(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    for (uint8_t i = 0; i < device_count; i++) {
        if (devices[i] != NULL && devices[i]->deinit != NULL) {
            if (devices[i]->deinit() != HAL_OK) {
                LOG_WARN(LOG_MOD_DEVICE, "Failed to deinitialize device: %s", 
                        devices[i]->name);
                status = HAL_ERROR;
            }
        }
    }

    return status;
}

/**
 * @brief Print device status for debugging
 */
void DeviceManager_PrintStatus(void)
{
    LOG_INFO(LOG_MOD_DEVICE, "=== Device Status Report ===");
    LOG_INFO(LOG_MOD_DEVICE, "Total devices: %d", device_count);
    
    for (uint8_t i = 0; i < device_count; i++) {
        if (devices[i] != NULL) {
            const char *status_str;
            switch (devices[i]->status) {
                case DEVICE_STATUS_OK:        status_str = "OK"; break;
                case DEVICE_STATUS_BUSY:      status_str = "BUSY"; break;
                case DEVICE_STATUS_ERROR:     status_str = "ERROR"; break;
                case DEVICE_STATUS_TIMEOUT:   status_str = "TIMEOUT"; break;
                case DEVICE_STATUS_NOT_READY: status_str = "NOT_READY"; break;
                default:                      status_str = "UNKNOWN"; break;
            }
            LOG_INFO(LOG_MOD_DEVICE, "  [%d] %s: %s", i, devices[i]->name, status_str);
        }
    }
}
