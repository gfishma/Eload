/**
 ******************************************************************************
 * @file           : app_logger.c
 * @brief          : Application logging implementation
 ******************************************************************************
 */

#include "app_logger.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdarg.h>
#include <string.h>

/* ============================= Private Variables ============================ */
static LogLevel_t current_log_level = LOG_LEVEL_INFO;

/* ============================= Private Functions ============================ */
static const char* Logger_GetLevelString(LogLevel_t level)
{
    switch (level) {
        case LOG_LEVEL_DEBUG:    return "DBG";
        case LOG_LEVEL_INFO:     return "INF";
        case LOG_LEVEL_WARNING:  return "WRN";
        case LOG_LEVEL_ERROR:    return "ERR";
        case LOG_LEVEL_CRITICAL: return "CRT";
        default:                 return "UNK";
    }
}

/* ============================= Public Functions ============================= */

/**
 * @brief Initialize logger
 */
void Logger_Init(LogLevel_t initial_level)
{
    current_log_level = initial_level;
    LOG_INFO(LOG_MOD_CONFIG, "Logger initialized at level %d", initial_level);
}

/**
 * @brief Set log level filter
 */
void Logger_SetLevel(LogLevel_t level)
{
    if (level >= LOG_LEVEL_DEBUG && level <= LOG_LEVEL_NONE) {
        current_log_level = level;
    }
}

/**
 * @brief Log message with specified level and module
 */
void Logger_Log(LogLevel_t level, const char *module, const char *format, ...)
{
    /* Filter by current log level */
    if (level < current_log_level) {
        return;
    }

    /* Check parameters */
    if (module == NULL || format == NULL) {
        return;
    }

    va_list args;
    char buffer[LOG_MESSAGE_SIZE];

    /* Format the message */
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    /* Print with timestamp, level, and module in a critical section to prevent interleaving */
    taskENTER_CRITICAL();
    printf("[%s] %s: %s\r\n", Logger_GetLevelString(level), module, buffer);
    taskEXIT_CRITICAL();
}
