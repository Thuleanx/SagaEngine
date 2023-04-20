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

    PLOG_VERBOSE << "This is a VERBOSE message";
    PLOG_DEBUG << "This is a DEBUG message";
    PLOG_INFO << "This is an INFO message";
    PLOG_WARNING << "This is a WARNING message";
    PLOG_ERROR << "This is an ERROR message";
    PLOG_FATAL << "This is a FATAL message";

	SINFO("Logger initialized.");
	return true;
}

void shutDownLogger() {
	SINFO("Logger shut down.");
	// TODO: cleanup any logging for queued entries
	fflush(stdout);
	fflush(stderr);
}


} // namespace Saga
