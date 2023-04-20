#pragma once
#include "../defines.h"
#include <string>
#include "plog/Log.h"

// from https://www.youtube.com/watch?v=l9e8PJskYnI&list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj&index=5

namespace Saga {

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if SAGA_RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

/**
 * @brief Define multiple levels of logging. Fatal and Error are always enabled, and debug and traced are left out of any build with SAGA_RELEASE defined.
 * 
 */
enum LogLevel {
	FATAL = 0, //!< Reserved for anything that would cause the core engine to crash. This should force a shutdown to prevent any data corruption or loss. Assertions raise this type of log level.
	ERROR = 1, //!< Reserved for anything that will cause the application to crash, or create undefined behaviour. (null references, etc). 
	WARN = 2, //!< Anything that can cause oddities with the application's execution. This might or might not result in an error.
	INFO = 3, //!< Generally useful information to log, like successful initialization or completion of significant tasks.
	DEBUG = 4, //!< Fine-grained information events that is useful to debug the application.
	TRACE = 5 //!< Finer-grained information, typically targetted towards one part of the application.
};

/**
 * @brief Initialize the logger. Neccessary to call before any logging.
 * 
 * @return true 
 * @return false 
 */
bool initializeLogger();

/**
 * @brief Log an output.
 * 
 * @param level level of the output. 
 * @see LogLevel
 * @param message message of the output, as a formatted string similar to printf.
 * @param ... any arguments to the formatted string.
 */
void logOutput(LogLevel level, const char* message, ...);

/**
 * @brief Shut down the logger, and flush any buffered output.
 */
void shutDownLogger();

#ifndef SFATAL
/**
 * @brief Log fatal-level messages.
 * @param message formatted string, similar to printf.
 */
#define SFATAL(message, ...) PLOG(plog::fatal).printf(message, ##__VA_ARGS__)
#endif

#ifndef SERROR
/**
 * @brief Log error-level messages.
 * @param message formatted string, similar to printf.
 */
#define SERROR(message, ...) PLOG(plog::error).printf(message, ##__VA_ARGS__)
#endif

#if LOG_WARN_ENABLED == 1
#ifndef SWARN
/**
 * @brief Log warn-level messages.
 * @param message formatted string, similar to printf.
 */
#define SWARN(message, ...) PLOG(plog::warning).printf(message, ##__VA_ARGS__)
#endif
#else
#define SWARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
#ifndef SINFO
/**
 * @brief Log info-level messages.
 * @param message formatted string, similar to printf.
 */
#define SINFO(message, ...) PLOG(plog::info).printf(message, ##__VA_ARGS__)
#endif
#else
#define SINFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#ifndef SDEBUG
/**
 * @brief Log debug-level messages.
 * @param message formatted string, similar to printf.
 */
#define SDEBUG(message, ...) PLOG(plog::debug).printf(message, ##__VA_ARGS__)
#endif
#else
#define SDEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#ifndef STRACE
/**
 * @brief Log trace-level messages.
 * @param message formatted string, similar to printf.
 */
#define STRACE(message, ...) PLOG(plog::verbose).printf(message, ##__VA_ARGS__)
#endif
#else
#define STRACE(message, ...)
#endif

}
