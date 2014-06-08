
#ifndef _CUSTOM_HEADERS_H
#define _CUSTOM_HEADERS_H

//Defines custom (debug, log, ...)
#include "defines.hpp"

 #if defined (__APPLE__) || defined(MACOSX)
static const char* CL_GL_SHARING_EXT = "cl_APPLE_gl_sharing";
#else
static const char* CL_GL_SHARING_EXT ="cl_khr_gl_sharing";
#endif

//ORDRE IMPORTANT GL CL QT CL-GL GLX CL-CXX
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
#include <QtGui>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMenuBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGraphicsView>
#include <QGLWidget>
#include <QGLFormat>
#include <QGLContext>
#include <QGraphicsScene>
#include <QPainter>
#include <QPaintEngine>
#include <QRect>
#include <QRectF>
#include <QResizeEvent>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QGenericMatrix>

//GLX après QT (don't ask why)
#include <CL/cl_gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#pragma GCC diagnostic pop
//////////////////////////


//Headers utilitaires
#include "errorCodes.hpp"
#include "log.hpp"
#include "utils.hpp"
#include "clUtils.hpp"


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

//print en multithread
#define PRINT_ONCE(mess) {std::once_flag flag; std::call_once(flag, [](){std::cout << mess << std::endl;});}
		
	

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
