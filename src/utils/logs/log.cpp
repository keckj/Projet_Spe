
#include "defines.hpp"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>

#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"

namespace log4cpp {

	Category *log_console = &Category::getRoot();
	Category *log_file = &Category::getInstance(std::string("log_file"));

	void initLogs() {

		log_console->setPriority(Priority::__CONSOLE_LOG_LEVEL);
		log_file->setPriority(Priority::__FILE_LOG_LEVEL);

		log4cpp::Appender *appender_console = new log4cpp::OstreamAppender("console", &std::cout);
		appender_console->setLayout(new log4cpp::BasicLayout());
		appender_console->setThreshold(Priority::DEBUG);
		log_console->addAppender(appender_console);

		log4cpp::Appender *appender_file = new log4cpp::FileAppender("default", "program.log");
		appender_file->setLayout(new log4cpp::BasicLayout());
		appender_file->setThreshold(Priority::DEBUG);
		//log_file->addAppender(appender_file);

		//console redirected to file
		log_console->addAppender(appender_file);

	}
}

