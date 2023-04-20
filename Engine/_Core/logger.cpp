#include "logger.h"

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <cstdarg>
#include "plog/Appenders/ColorConsoleAppender.h"
#include "plog/Formatters/TxtFormatter.h"
#include "plog/Init.h"
#include "plog/Severity.h"
#include "plog/Initializers/RollingFileInitializer.h"

namespace Saga {

bool initializeLogger() {
	// TODO: create log file
    static plog::ColorConsoleAppender<plog::TxtFormatter> appender;
    plog::init(plog::Severity::debug, "sagaruntime.log").addAppender(&appender);
	SINFO("Logger initialized.");
	return true;
}

void shutDownLogger() {
	SINFO("Logger shut down.");
	// TODO: cleanup any logging for queued entries
	fflush(stdout);
	fflush(stderr);
}

void logOutput(LogLevel level, const char* message, ...) {
	const char* LEVEL_STR[6] = {
		"[FATAL]: ", 
		"[ERROR]: ",
		"[WARN]: ",
		"[INFO]: ",
		"[DEBUG]: ",
		"[TRACE]: "
	};
	bool isError = level < 2;

	char outMessage[32000] = {0};

	// put the variadic arguments into the message.
	va_list arg_ptr;
    va_start(arg_ptr, message);
	vsnprintf(outMessage, 32000, message, arg_ptr);
	va_end(arg_ptr);

    // prepend the error level to the message.
	char outMessage2[32000] = {0};
	sprintf(outMessage2, "%s%s\n", LEVEL_STR[level], outMessage);

	if (isError) {
		#ifdef SAGA_DEBUG
			fflush(stdout);
			fflush(stderr);
		#endif
		fprintf(stderr, "%s", outMessage2);
		#ifdef SAGA_DEBUG
			fflush(stderr);
		#endif
	}
	else {
		#ifdef SAGA_DEBUG
			fflush(stdout);
			fflush(stderr);
		#endif
		fprintf(stdout, "%s", outMessage2);
		#ifdef SAGA_DEBUG
			fflush(stdout);
		#endif
	}

}

} // namespace Saga
