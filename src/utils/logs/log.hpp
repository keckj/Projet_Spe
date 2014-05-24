
#ifndef __LOG_H
#define __LOG_H

#include "log4cpp/Category.hh"

namespace log4cpp {
	extern Category *log_console;
	extern Category *log_file;

	void initLogs();
}

#endif /* end of include guard: LOG_H */
