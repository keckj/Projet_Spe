
#include "headers.hpp"
#include "utils.hpp"

#include <cassert>
#include <ctime>
#include <vector>
#include <iostream>

using namespace log4cpp;
using namespace utils;

int main(int argc, char** argv) {

	//Init
	log4cpp::initLogs();
    srand(time(NULL));
	log_console->infoStream() << "Program launch !";
	log_console->infoStream() << "[Logs Init] ";
	log_console->infoStream() << "[Rand Init] ";
    //log_console->infoStream() << "[Glut Init] ";
    //glutInit(&argc, argv);
	//log_console->infoStream() << "[Qt Init] ";
	//QApplication application(argc,argv);
	//log_console->infoStream() << "[Glew Init] " << glewGetErrorString(glewInit());
	//log_console->infoStream()<< "";

	//Check platforms, create contexts and load devices
	
	cl_int err;

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

	//Load sources
	cl::Program::Sources sources = utils::loadSourcesFromFile("src/kernels/demo.cl");	

	//Make program
	cl::Program program(gpuContexts[0], sources, &err); CHK_ERRORS(err);
	
	utils::buildProgram(program, gpuDevices[0], "", "demo.cl");

	//Make kernel
	cl::Kernel kernel(program, "vectorAdd");
	
	//Create a command queue for first gpu device from first gpu context
	cl::CommandQueue queue(gpuContexts[0], gpuDevices[0][0]);

	//Buffers
	cl::Buffer bufferA(gpuContexts[0], CL_MEM_READ_ONLY, 16 * sizeof(int));
	cl::Buffer bufferB(gpuContexts[0], CL_MEM_READ_ONLY, 16 * sizeof(int));
	cl::Buffer bufferC(gpuContexts[0], CL_MEM_WRITE_ONLY, 16 * sizeof(int));

	int *dataA = new int[16];
	int *dataB = new int[16];
	for (int i = 0; i < 16; i++) {
		dataA[i] = i;
		dataB[i] = 16-i;
	}

	CHK_ERROR_RET(queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, 16 * sizeof(int), dataA));
	CHK_ERROR_RET(queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0, 16 * sizeof(int), dataB));

	kernel.setArg(0, bufferA);
	kernel.setArg(1, bufferB);
	kernel.setArg(2, bufferC);

	cl::NDRange global(16);
	cl::NDRange local(1);
	CHK_ERROR_RET(queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local));

	int *dataC = new int[16];
	CHK_ERROR_RET(queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, 16*sizeof(int), dataC));

	for(int i = 0; i < 16; i++)
             std::cout << dataA[i] << " + " << dataB[i] << " = " << dataC[i] << std::endl;


	return EXIT_SUCCESS;
}
