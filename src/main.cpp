
#include <Python.h>
#include "headers.hpp"

#include "mainWindow.hpp"
#include "globals.hpp"
#include "model.hpp"

#include <iostream>

#include "colormap.hpp"

#if _N_MAIN == 0

using namespace log4cpp;
using namespace utils;

int main(int argc, char** argv) {

	//INIT
	log4cpp::initLogs();
	log_console->infoStream() << "Program launch !";
	log_console->infoStream() << "[Logs Init] ";

	log_console->infoStream() << "[Rand Init] ";
    srand(time(NULL));
	
	log_console->infoStream() << "[Python Init] ";
	Py_Initialize();

	log_console->infoStream() << "[XThreads Init] ";
	XInitThreads();

    log_console->infoStream() << "[Glut Init] ";
    glutInit(&argc, argv);

	log_console->infoStream() << "[Qt Init] ";
	QApplication application(argc,argv);
    MainWindow win;
    win.show();
	//FIN INIT (le reste de GL est fait dans QT)//
	
	application.exec();
	Py_Finalize();

	glXMakeCurrent(Model::solverDisplay, 0, 0);
	glXDestroyContext(Model::solverDisplay, Model::solverContext);
	XDestroyWindow(Model::solverDisplay, Model::solverWindow);
	XFreeColormap(Model::solverDisplay, Model::solverColormap);
	XCloseDisplay(Model::solverDisplay);

	return EXIT_SUCCESS;
}

#endif /* _N_MAIN == 0 */
