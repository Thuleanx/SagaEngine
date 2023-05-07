#include "logger.h"

#include <fstream>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <cstdarg>
#include "plog/Appenders/ColorConsoleAppender.h"
#include "plog/Formatters/TxtFormatter.h"
#include "plog/Formatters/FuncMessageFormatter.h"
#include "plog/Init.h"
#include "plog/Severity.h"
#include "plog/Initializers/RollingFileInitializer.h"

namespace Saga {

bool initializeLogger() {
    // clear the log file
    std::ofstream ofs;
    ofs.open("sagaruntime.log", std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    // open the log file again for output
    static plog::ColorConsoleAppender<plog::TxtFormatter> appender;
    plog::init(plog::Severity::verbose, "sagaruntime.log").addAppender(&appender);

    /* PLOG_VERBOSE << "This is a VERBOSE message"; */
    /* PLOG_DEBUG << "This is a DEBUG message"; */
    /* PLOG_INFO << "This is an INFO message"; */
    /* PLOG_WARNING << "This is a WARNING message"; */
    /* PLOG_ERROR << "This is an ERROR message"; */
    /* PLOG_FATAL << "This is a FATAL message"; */

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
