
#include "headers.hpp"

#include "utils.hpp"
#include "mainWindow.hpp"

#include "grid1D.hpp"
#include "grid2D.hpp"
#include "grid3D.hpp"
#include "simpleSystem.hpp"
#include "solver.hpp"

#include <cassert>
#include <ctime>
#include <vector>
#include <iostream>
#include <map>

#if _N_MAIN == 0

using namespace log4cpp;
using namespace utils;

int main(int argc, char** argv) {

	//Init
	log4cpp::initLogs();
    srand(time(NULL));
	log_console->infoStream() << "Program launch !";
	log_console->infoStream() << "[Logs Init] ";
	log_console->infoStream() << "[Rand Init] ";
    log_console->infoStream() << "[Glut Init] ";
    glutInit(&argc, argv);
	log_console->infoStream() << "[Qt Init] ";
	QApplication application(argc,argv);

	log_console->infoStream() << "[Glew Init] " << glewGetErrorString(glewInit());
	log_console->infoStream()<< "";
    
    //qRegisterMetaType< Grid2D<float> >("Grid2D<float>");
    //qRegisterMetaType< Grid3D<float> >("Grid3D<float>");
    MainWindow win;
    win.show();
    
    return application.exec();

}

#endif /* _N_MAIN == 0 */
