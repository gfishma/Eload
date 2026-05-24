/**
 ******************************************************************************
 * @file           : CODING_GUIDELINES.h
 * @brief          : Code structure optimization guidelines and best practices
 * @version        : 1.0
 * @date           : 2024
 ******************************************************************************
 *
 * OVERVIEW
 * ========
 * This document outlines the optimized code structure and best practices
 * for the STM32F407 ELoad Control project.
 *
 * The optimization focuses on:
 * 1. Centralized Configuration Management
 * 2. Unified Logging and Debugging System
 * 3. Device Driver Abstraction Layer
 * 4. Decoupled Task Architecture
 * 5. Organized Include Hierarchy
 *
 * ============================================================================
 * SECTION 1: MODULE ORGANIZATION
 * ============================================================================
 *
 * Core/Inc/
 *   ├── config.h                 : Global configuration & constants
 *   ├── app_logger.h             : Logging system
 *   ├── device_manager.h         : Device abstraction layer
 *   ├── app_tasks.h              : Task definitions & interfaces
 *   ├── app_includes.h           : Unified include header
 *   ├── main.h                   : MCU main definitions
 *   └── [peripheral_*.h]         : HAL peripheral headers (auto-generated)
 *
 * Core/Src/
 *   ├── config.c                 : Configuration implementation
 *   ├── app_logger.c             : Logger implementation
 *   ├── device_manager.c         : Device manager implementation
 *   ├── app_tasks.c              : Task implementations
 *   ├── freertos.c               : FreeRTOS kernel integration (refactored)
 *   ├── main.c                   : Application entry point
 *   └── [peripheral_*.c]         : HAL peripheral implementations
 *
 * ============================================================================
 * SECTION 2: CONFIGURATION MANAGEMENT
 * ============================================================================
 *
 * USE: config.h
 * BENEFITS:
 *   ✓ Single source of truth for all constants
 *   ✓ Easy to modify parameters without code changes
 *   ✓ Centralized threshold and limit definitions
 *   ✓ Better readability with organized sections
 *
 * EXAMPLE:
 *   #define CFG_MAX_CURRENT_MA          13000
 *   #define CFG_TEMP_PROTECT_THRESHOLD  70.0f
 *   #define CFG_TASK_OLED_STACK_SIZE    512
 *
 * ACCESS:
 *   float max_current = Config_GetMaxCurrent();
 *   float threshold = Config_GetTempThreshold();
 *
 * ============================================================================
 * SECTION 3: LOGGING SYSTEM
 * ============================================================================
 *
 * USE: app_logger.h
 * BENEFITS:
 *   ✓ Centralized debug output
 *   ✓ Filterable by log level (DEBUG, INFO, WARN, ERROR, CRITICAL)
 *   ✓ Module-based filtering
 *   ✓ Easy enable/disable for production
 *   ✓ Consistent log format
 *
 * USAGE:
 *   LOG_INFO(LOG_MOD_TASK, "Task started");
 *   LOG_ERROR(LOG_MOD_DEVICE, "Device init failed");
 *   LOG_DEBUG(LOG_MOD_HAL, "ADC value: %d", adc_val);
 *
 * SET LOG LEVEL:
 *   Logger_Init(LOG_LEVEL_INFO);      // Only INFO and above
 *   Logger_SetLevel(LOG_LEVEL_DEBUG); // Show all messages
 *
 * MODULE NAMES:
 *   LOG_MOD_TASK      - Task execution
 *   LOG_MOD_DEVICE    - Device management
 *   LOG_MOD_HAL       - Hardware abstraction
 *   LOG_MOD_CONFIG    - Configuration system
 *   LOG_MOD_KEY       - Keypad input
 *   LOG_MOD_TEMP      - Temperature monitoring
 *
 * ============================================================================
 * SECTION 4: DEVICE MANAGER (Abstraction Layer)
 * ============================================================================
 *
 * USE: device_manager.h
 * BENEFITS:
 *   ✓ Unified interface for all devices
 *   ✓ Easy to add new devices
 *   ✓ Centralized device status tracking
 *   ✓ Simplified error handling
 *   ✓ Better testability
 *
 * DEVICE REGISTRATION:
 *   Device_t my_device = {
 *       .id = DEVICE_ID_DAC_AD5667,
 *       .name = "AD5667 DAC",
 *       .init = AD5667_Init,
 *       .deinit = AD5667_Deinit,
 *       .check_ready = AD5667_IsReady
 *   };
 *   DeviceManager_Register(&my_device);
 *
 * DEVICE USAGE:
 *   if (DeviceManager_IsReady(DEVICE_ID_DAC_AD5667) == HAL_OK) {
 *       // Device is ready to use
 *   }
 *   DeviceManager_PrintStatus();  // Debug all devices
 *
 * ============================================================================
 * SECTION 5: TASK ARCHITECTURE
 * ============================================================================
 *
 * USE: app_tasks.h
 * BENEFITS:
 *   ✓ Decoupled from FreeRTOS implementation details
 *   ✓ Business logic in separate functions
 *   ✓ Easy to unit test
 *   ✓ Reusable task components
 *   ✓ Clear task responsibilities
 *
 * TASK TYPES:
 *   TASK_OLED_CTRL          - Command parsing & current control
 *   TASK_DISPLAY_UPDATE     - LCD refresh
 *   TASK_KEYPAD_PROCESS    - Input handling
 *   TASK_DATA_ACQUISITION  - ADC sampling
 *   TASK_MONITOR_PROTECTION- Temperature protection
 *
 * TASK FUNCTIONS (Business Logic):
 *   Task_ParseCurrentValue()      - String parsing
 *   Task_ApplyCurrentSetting()    - Hardware control
 *   Task_HandleKeySet()           - Input handler
 *   Task_ReadTemperature()        - Sensor reading
 *   Task_CheckTemperatureProtection() - Safety check
 *
 * TASK IMPLEMENTATION:
 *   void MyTask(void *argument) {
 *       TaskContext_t *ctx = Tasks_GetContext(TASK_OLED_CTRL);
 *       
 *       for (;;) {
 *           // Call business logic functions
 *           Task_ApplyCurrentSetting(value);
 *           
 *           osDelay(100);  // Or appropriate delay
 *       }
 *   }
 *
 * ============================================================================
 * SECTION 6: INCLUDE HIERARCHY
 * ============================================================================
 *
 * NEW WAY - Single unified include:
 *
 *   #include "app_includes.h"  // One include gets everything!
 *
 * HIERARCHY:
 *   ┌─────────────────────────────────────┐
 *   │ HAL & CMSIS Foundation              │
 *   │ (stm32f4xx_hal.h, cmsis_os.h)       │
 *   └─────────────────────────────────────┘
 *                    ↓
 *   ┌─────────────────────────────────────┐
 *   │ FreeRTOS Kernel                     │
 *   │ (FreeRTOS.h, task.h)                │
 *   └─────────────────────────────────────┘
 *                    ↓
 *   ┌─────────────────────────────────────┐
 *   │ Application Framework               │
 *   │ (config, logger, device_manager)    │
 *   └─────────────────────────────────────┘
 *                    ↓
 *   ┌─────────────────────────────────────┐
 *   │ MCU Peripheral HAL                  │
 *   │ (adc.h, uart.h, spi.h, etc.)       │
 *   └─────────────────────────────────────┘
 *                    ↓
 *   ┌─────────────────────────────────────┐
 *   │ Device Drivers (iCore)              │
 *   │ (Tftlcd.h, adc_dma.h, etc.)         │
 *   └─────────────────────────────────────┘
 *                    ↓
 *   ┌─────────────────────────────────────┐
 *   │ Application Specific                │
 *   │ (scmd_dvm.h, Module_DVM_V2.h)       │
 *   └─────────────────────────────────────┘
 *
 * ============================================================================
 * SECTION 7: CODING PATTERNS
 * ============================================================================
 *
 * PATTERN 1: Configuration Access
 * ────────────────────────────────
 *   // Instead of: #define MAX_CURRENT 13000
 *   // Use:
 *   uint16_t max = Config_GetMaxCurrent();
 *   
 *   // In config.h:
 *   #define CFG_MAX_CURRENT_MA 13000
 *   uint16_t Config_GetMaxCurrent(void) { return CFG_MAX_CURRENT_MA; }
 *
 * PATTERN 2: Logging Instead of Printf
 * ──────────────────────────────────────
 *   // Instead of: printf("Error: %d\r\n", error_code);
 *   // Use:
 *   LOG_ERROR(LOG_MOD_DEVICE, "Device error: %d", error_code);
 *   
 *   // Can disable all logs in one place
 *   Logger_SetLevel(LOG_LEVEL_NONE);  // Production mode
 *
 * PATTERN 3: Device Access
 * ────────────────────────
 *   // Instead of: if (AD5667_IsReady() != HAL_OK) { ... }
 *   // Use:
 *   if (DeviceManager_IsReady(DEVICE_ID_DAC_AD5667) != HAL_OK) {
 *       LOG_ERROR(LOG_MOD_DEVICE, "DAC not ready");
 *       return HAL_ERROR;
 *   }
 *
 * PATTERN 4: Task Business Logic
 * ──────────────────────────────
 *   // In freertos.c:
 *   void MyTask(void *argument) {
 *       for (;;) {
 *           Task_ApplyCurrentSetting(value);  // Call business logic
 *           osDelay(100);
 *       }
 *   }
 *   
 *   // Implementation in app_tasks.c:
 *   HAL_StatusTypeDef Task_ApplyCurrentSetting(float current_ma) {
 *       // Business logic here - can be unit tested
 *       if (current_ma > CFG_MAX_CURRENT_MA) {
 *           LOG_ERROR(LOG_MOD_TASK, \"Current out of range\");
 *           return HAL_ERROR;
 *       }
 *       // ... actual implementation
 *   }
 *
 * ============================================================================
 * SECTION 8: MIGRATION CHECKLIST
 * ============================================================================
 *
 * Step 1: Update includes in existing files
 *   □ Replace scattered includes with #include \"app_includes.h\"
 *   □ Verify compilation
 *
 * Step 2: Register devices
 *   □ Add device structs to device_manager initialization
 *   □ Test device initialization
 *
 * Step 3: Replace printf with logging
 *   □ Find all printf() calls
 *   □ Replace with LOG_INFO/ERROR/WARN/DEBUG
 *   □ Test log output
 *
 * Step 4: Extract constants to config.h
 *   □ Find all #define for constants
 *   □ Move to config.h with CFG_ prefix
 *   □ Update code to use Config_Get*() functions
 *
 * Step 5: Refactor tasks
 *   □ Extract task business logic to app_tasks.c
 *   □ FreeRTOS integration remains in freertos.c
 *   □ Keep task loop simple: call functions + osDelay()
 *
 * ============================================================================
 * SECTION 9: BENEFITS SUMMARY
 * ============================================================================
 *
 * ✓ MAINTAINABILITY
 *   - Clear module organization
 *   - Easy to find code
 *   - Reduced coupling
 *   - Single responsibility per module
 *
 * ✓ DEBUGGABILITY
 *   - Centralized logging
 *   - Device status tracking
 *   - Task execution statistics
 *   - Easy log level switching
 *
 * ✓ SCALABILITY
 *   - Easy to add new devices
 *   - Easy to add new tasks
 *   - Easy to add new configurations
 *   - Device manager handles complexity
 *
 * ✓ TESTABILITY
 *   - Business logic separated from RTOS
 *   - Device-independent code
 *   - Mock-friendly interfaces
 *   - Configuration-driven behavior
 *
 * ✓ CODE REUSE
 *   - Shared utilities (logger, device manager)
 *   - Common patterns (config access, logging)
 *   - Task components usable in multiple tasks
 *   - Drivers register once, use anywhere
 *
 * ✓ PERFORMANCE
 *   - Minimal overhead (macro-based logging)
 *   - Efficient device lookup
 *   - No unnecessary abstraction layers
 *   - Configurable log levels for production
 *
 * ============================================================================
 */

#endif /* __CODING_GUIDELINES_H__ */
