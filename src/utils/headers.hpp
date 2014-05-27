
#ifndef _CUSTOM_HEADERS_H
#define _CUSTOM_HEADERS_H


//Defines custom (debug, log, ...)
#include "defines.hpp"

//ORDRE IMPORTANT GL CL QT
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#include <GL/glew.h>

#if defined(__APPLE__) || defined(__MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//wrapper C++ pour opencl 1.1 (nvidia supporte pas mieux !) 
#include "utils/opencl/cl_1_1.hpp"

// QT
#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#pragma GCC diagnostic pop
//////////////////////////


//Headers utilitaires
#include "errorCodes.hpp"
#include "log.hpp"
#include "utils.hpp"


//MACROS ERREURS OPENCL//
//A utiliser pour check les erreurs de retour de fonctions
#ifdef __DEBUG
#define CHK_ERROR_RET(ans) { clAssert((ans), __FILE__, __LINE__); }
#else
#define CHK_ERROR_RET(ans) {ans}
#endif

//A utiliser pour check les erreurs sur une variable de type cl_int err;
#ifdef __DEBUG
#define CHK_ERRORS(ans) { clAssert((ans), __FILE__, __LINE__); }
#else
#define CHK_ERRORS(ans) {;}
#endif

//Si jamais on veux des exceptions non bloquantes
#ifdef __CL_ENABLE_EXCEPTIONS
#define TRY(ans) {try{ans} catch(cl::Error e) {log_console->errorStream() << e.what() << "\t" << openCLGetErrorString(e.err());};}
#else
#define TRY(ans) {ans}
#endif
	

inline void clAssert(cl_int err, const std::string &file, int line, bool abort = true) {
	if (err != CL_SUCCESS)
	{
		log4cpp::log_console->errorStream() << "OpenCL assert false :\n\t\t" 
			<< openCLGetErrorString(err) << " in file " <<  file << ":" << line << ".";

		if (abort) 
			exit(EXIT_FAILURE);
	}
}
////////////////////////

#endif /* end of include guard: _CUSTOM_HEADERS_H */
