/**
 ******************************************************************************
 * @file           : app_includes.h
 * @brief          : Centralized application includes management
 * @purpose        : Unified include hierarchy for the entire application
 * @usage          : Include this file instead of multiple scattered includes
 ******************************************************************************
 */

#ifndef __APP_INCLUDES_H__
#define __APP_INCLUDES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ============================= HAL & CMSIS ================================== */
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

/* ============================= FreeRTOS ==================================== */
#include "FreeRTOS.h"
#include "task.h"

/* ============================= Standard C Library =========================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================= Application Framework ======================== */
#include "config.h"
#include "app_logger.h"
#include "device_manager.h"
#include "app_tasks.h"

/* ============================= MCU Peripheral HAL ============================= */
#include "adc.h"
#include "can.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* ============================= Device Drivers (iCore) ======================== */
#include "Tftlcd.h"
#include "adc_dma.h"
#include "pwm.h"
#include "24cxx.h"
#include "ad5667.h"
#include "char_int.h"
#include "Temp.h"
#include "mcp3421.h"

/* ============================= Application Specific ========================== */
#include "scmd_dvm.h"
#include "Module_DVM_V2.h"
#include "i_usart.h"
#include "stm32f4xx_it.h"

/* ============================= Uncomment to Enable ============================= */
/* #include "cat9555.h" */  /* CAT9555 I2C port expander - if used */

/* ============================= Include Order Notes ============================
 *
 * 1. HAL & CMSIS Foundation
 * 2. FreeRTOS kernel
 * 3. Standard C library
 * 4. Application framework (config, logger, device manager)
 * 5. MCU peripheral HAL layer
 * 6. Device drivers (iCore)
 * 7. Application-specific modules
 *
 * This hierarchy ensures:
 * - Lower layers don't depend on higher layers
 * - Minimal circular dependencies
 * - Easy to understand dependency graph
 * - Easy to identify missing dependencies
 *
 * ========================= Include Optimization Tips ==========================
 *
 * OLD WAY (scattered):
 *    #include "FreeRTOS.h"
 *    #include "task.h"
 *    #include "stm32f4xx_hal.h"
 *    #include "config.h"
 *    #include "Tftlcd.h"
 *    ... many more includes
 *
 * NEW WAY (unified):
 *    #include "app_includes.h"  // One include gets everything
 *
 * ========================= Layer Definitions =================================
 *
 * Layer 1 (Foundation): HAL, CMSIS, FreeRTOS
 * Layer 2 (Framework): Config, Logger, Device Manager
 * Layer 3 (Hardware): Peripheral HAL (ADC, DAC, UART, etc.)
 * Layer 4 (Drivers): Device drivers (LCD, ADC_DMA, Sensors, etc.)
 * Layer 5 (Application): Business logic, tasks, protocols
 *
 * Dependencies flow downward (Layer 5 → Layer 1)
 * Never flow upward or sideways between same-level modules
 *
 ******************************************************************************
 */

#ifdef __cplusplus
}
#endif

#endif /* __APP_INCLUDES_H__ */
