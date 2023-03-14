#pragma once
#include "../defines.h"

namespace Saga {

// Comment the line below to disable assertions
#define SAGA_ASSERTIONS_ENABLED

#ifdef SAGA_ASSERTIONS_ENABLED

/*
	We do this so that this is compatible with different compilers. VSCode's MSC will complain if we use builtin_trap, since that's clang specific. But Qt uses builtin_trap
*/
#if _MSC_VER
#include <intrin.h>
/**
 * @brief Break out of the program
 */
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

/**
 * @brief Output a message signaling that an assertion has failed. This message is of the form:
 * Assertion Failure: #expression, message: `message`, in file: `file`, line: `line`
 *
 * @param expression a string representation of the expression that failed the assertion.
 * @param message the message to describe the assertion failure.
 * @param file the file the error happened in.
 * @param line the line number the error happened in.
 */
void reportAssertionFailed(const char *expression, const char *message, const char *file, int line, ...);

/**
 * @brief Assert an expression evaluates to true. If it does not, log a FATAL level message and stop the execution of the program.
 * @param expr an expression that evaluates to a boolean.
 */
#define SASSERT(expr)                                             \
	{                                                             \
		if (!(expr)) {                                            \
			reportAssertionFailed(#expr, "", __FILE__, __LINE__); \
			debugBreak();                                         \
		}                                                         \
	}

/**
 * @brief Assert an expression evaluates to true. If it does not, log a FATAL level message and stop the execution of the program.
 * @param expr an expression that evaluates to a boolean.
 * @param message a message that will be displayed when an error happens.
 */
#define SASSERT_MESSAGE(expr, message, ...)                                           \
	{                                                                                 \
		if (!(expr)) {                                                                \
			reportAssertionFailed(#expr, message, __FILE__, __LINE__, ##__VA_ARGS__); \
			debugBreak();                                                             \
		}                                                                             \
	}

#ifdef SAGA_DEBUG
/**
 * @brief Assert an expression evaluates to true. If it does not, log a FATAL level message and stop the execution of the program. Only works if @em SAGA_DEBUG is enabled.
 * @param expr an expression that evaluates to a boolean.
 */
#define SASSERT_DEBUG(expr)                                       \
	{                                                             \
		if (!(expr)) {                                            \
			reportAssertionFailed(#expr, "", __FILE__, __LINE__); \
			debugBreak();                                         \
		}                                                         \
	}

/**
 * @brief Assert an expression evaluates to true. If it does not, log a FATAL level message and stop the execution of the program. Only works if @em SAGA_DEBUG is enabled.
 * @param expr an expression that evaluates to a boolean.
 * @param message a message that will be displayed when an error happens.
 */
#define SASSERT_DEBUG_MESSAGE(expr, message, ...)                                     \
	{                                                                                 \
		if (!(expr)) {                                                                \
			reportAssertionFailed(#expr, message, __FILE__, __LINE__, ##__VA_ARGS__); \
			debugBreak();                                                             \
		}                                                                             \
	}
#else
#define SASSERT_DEBUG(expr)                  // does nothing
#define SASSERT_DEBUG_MESSAGE(expr, message) // does nothing
#endif

#else
#define SASSERT(expr)                        // does nothing
#define SASSERT(expr, message)               // does nothing
#define SASSERT_DEBUG(expr)                  // does nothing
#define SASSERT_DEBUG_MESSAGE(expr, message) // does nothing
#endif

} // namespace Saga