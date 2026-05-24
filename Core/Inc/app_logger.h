/**
 ******************************************************************************
 * @file           : app_logger.h
 * @brief          : Application logging and debugging system
 ******************************************************************************
 */

#ifndef __APP_LOGGER_H__
#define __APP_LOGGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

/* ============================= Log Levels ================================== */
typedef enum {
    LOG_LEVEL_DEBUG     = 0,
    LOG_LEVEL_INFO      = 1,
    LOG_LEVEL_WARNING   = 2,
    LOG_LEVEL_ERROR     = 3,
    LOG_LEVEL_CRITICAL  = 4,
    LOG_LEVEL_NONE      = 5
} LogLevel_t;

/* ============================= Macros ======================================= */
#define LOG_MODULE_NAME_SIZE    16
#define LOG_MESSAGE_SIZE        128

/**
 * @brief Initialize logger
 * @param initial_level: Initial log level filter
 */
void Logger_Init(LogLevel_t initial_level);

/**
 * @brief Set log level filter
 * @param level: New log level
 */
void Logger_SetLevel(LogLevel_t level);

/**
 * @brief Log message with specified level and module
 * @param level: Log level
 * @param module: Module name (e.g., "TASK", "DEV", "HAL")
 * @param format: Format string (printf-like)
 */
void Logger_Log(LogLevel_t level, const char *module, const char *format, ...);

/* ============================= Convenience Macros =========================== */
#define LOG_DEBUG(module, format, ...)   Logger_Log(LOG_LEVEL_DEBUG, module, format, ##__VA_ARGS__)
#define LOG_INFO(module, format, ...)    Logger_Log(LOG_LEVEL_INFO, module, format, ##__VA_ARGS__)
#define LOG_WARN(module, format, ...)    Logger_Log(LOG_LEVEL_WARNING, module, format, ##__VA_ARGS__)
#define LOG_ERROR(module, format, ...)   Logger_Log(LOG_LEVEL_ERROR, module, format, ##__VA_ARGS__)
#define LOG_CRITICAL(module, format, ...) Logger_Log(LOG_LEVEL_CRITICAL, module, format, ##__VA_ARGS__)

/* ============================= Module Names ================================= */
#define LOG_MOD_TASK        "TASK"
#define LOG_MOD_DEVICE      "DEV"
#define LOG_MOD_HAL         "HAL"
#define LOG_MOD_CONFIG      "CFG"
#define LOG_MOD_KEY         "KEY"
#define LOG_MOD_TEMP        "TEMP"

#ifdef __cplusplus
}
#endif

#endif /* __APP_LOGGER_H__ */
