
#include "headers.hpp"

#include <cassert>
#include <vector>
#include <iostream>

int main (void) {

	//Init
	initLogs();


	//Check platforms, create contexts and load devices
	log_console.infoStream() << "Listing platforms..";

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

	if(nTotGpuDevices == 0) {
		log_console.critStream() << "No GPUs were found on your computer, aborting.";
		exit(EXIT_FAILURE);
	}
	if(nTotCpuDevices == 0) {
		log_console.warnStream() << "No CPUs were found on your computer.";
	}

	return EXIT_SUCCESS;
}
