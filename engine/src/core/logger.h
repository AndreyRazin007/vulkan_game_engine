#pragma once

#include "defines.h"

#define LOG_WARNING_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

/* Disable debug and trace logging for release builds. */
#if FRELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} log_level;

b8 initializeLogging();
void shutdownLogging();

FAPI void logOutput(log_level level, const char *message, ...);

/* Logs a fatal-level message. */
#define FFATAL(message, ...) logOutput(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef FERROR
/* Logs an error-level message. */
#define FERROR(message, ...) logOutput(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_WARNING_ENABLED == 1
/* Logs a warning-level message. */
#define FWARNING(message, ...) logOutput(LOG_LEVEL_WARNING, message, ##__VA_ARGS__);
#else
/* Does nothing when LOG_WARN_ENABLED != 1 */
#define FWARNING(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
/* Logs a info-level message. */
#define FINFO(message, ...) logOutput(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
/* Does nothing when LOG_INFO_ENABLED != 1 */
#define FINFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
/* Logs a debug-level message. */
#define FDEBUG(message, ...) logOutput(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
/* Does nothing when LOG_DEBUG_ENABLED != 1 */
#define FDEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
/* Logs a trace-level message. */
#define FTRACE(message, ...) logOutput(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
/* Does nothing when LOG_TRACE_ENABLED != 1 */
#define FTRACE(message, ...)
#endif
