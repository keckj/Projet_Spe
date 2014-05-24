
#include "headers.hpp"

#include <cassert>
#include <ctime>
#include <vector>
#include <iostream>

using namespace log4cpp;

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

	//Check platforms, create contexts and load devices
	log_console->infoStream() << "Listing platforms..";

	std::vector<cl::Platform> platforms;	
	std::vector<cl::Context> gpuContexts, cpuContexts, accContexts;
	std::vector<std::vector<cl::Device>> gpuDevices, cpuDevices, accDevices;

	unsigned int nPlatforms;
	unsigned int nTotGpuDevices, nTotCpuDevices, nTotAccDevices;
	unsigned int *nGpuDevices, *nCpuDevices, *nAccDevices;

	utils::loadDevicesAndCreateContexts(platforms,
										gpuContexts, cpuContexts, accContexts,
										gpuDevices, cpuDevices, accDevices,
										nPlatforms,
										nTotGpuDevices, nTotCpuDevices, nTotAccDevices,
										&nGpuDevices, &nCpuDevices, &nAccDevices);
	
	//Check devices
	if(nTotGpuDevices == 0) {
		log_console->critStream() << "No GPUs were found on your computer, aborting.";
		exit(EXIT_FAILURE);
	}
	if(nTotCpuDevices == 0) {
		log_console->warnStream() << "No CPUs were found on your computer.";
	}

	//Create queues
	

	return EXIT_SUCCESS;
}
