#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace log4cpp;

namespace utils {

	const std::string toStringMemory(unsigned long bytes) {

		std::stringstream ss;

		const char prefix[] = {' ', 'K', 'M', 'G', 'T', 'P'};
		unsigned long val = 1;
		for (int i = 0; i < 6; i++) {
			if(bytes < 1024*val) {
				ss << round(100*(float)bytes/val)/100.0 << prefix[i] << 'B';
				break;
			}
			val *= 1024;
		}

		const std::string str(ss.str());
		return str;
	 }
	
	bool isExtensionSupported(const char *extList, const char *extension)
	{
		const char *start;
		const char *where, *terminator;

		where = strchr(extension, ' ');
		if (where || *extension == '\0')
			return false;

		for (start=extList;;) {
			where = strstr(start, extension);

			if (!where)
				break;

			terminator = where + strlen(extension);

			if ( where == start || *(where - 1) == ' ' )
				if ( *terminator == ' ' || *terminator == '\0' )
					return true;

			start = terminator;
		}

		return false;
	}
}

