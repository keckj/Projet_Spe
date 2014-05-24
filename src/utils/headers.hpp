
#ifndef _CUSTOM_HEADERS_H
#define _CUSTOM_HEADERS_H

#include "defines.hpp"

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include "errorCodes.hpp"
#include "log.hpp"
#include "utils.hpp"

#ifdef __DEBUG
#define CHK_ERROR_RET(ans) { clAssert((ans), __FILE__, __LINE__); }
#else
#define CHK_ERROR_RET(ans) ans
#endif

#ifdef __DEBUG
#define CHK_ERRORS(ans) { clAssert((ans), __FILE__, __LINE__); }
#else
#define CHK_ERRORS(ans) ;
#endif


inline void clAssert(cl_int err, const std::string &file, int line, bool abort = true) {
	if (err != CL_SUCCESS)
	{
		log_console.errorStream() << "OpenCL assert false :\n\t\t" << openCLGetErrorString(err) << " in file " <<  file << ":" << line << ".";

		if (abort) 
			exit(err);
	}
}

#endif /* end of include guard: _CUSTOM_HEADERS_H */
