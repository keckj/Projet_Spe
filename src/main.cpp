
#include "headers.hpp"

#include "mainWindow.hpp"
#include "globals.hpp"
#include "texture.hpp"

#include <iostream>

#include "colormap.hpp"
#include <Python.h>

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

	return EXIT_SUCCESS;
}

#endif /* _N_MAIN == 0 */
