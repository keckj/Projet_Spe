

#include "headers.hpp"
#include "utils.hpp"

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

#if _N_MAIN == 3

using namespace utils;

int main(int argc, char** argv) {
	log4cpp::initLogs();
	std::cout << "Je suis le main 3 !" << std::endl;

	unsigned int width=50u, height=50u;
	Grid2D<float> e(1.0,1.0,width,height);
	Grid2D<float> r(1.0,1.0,width,height);

	float r2 = 0.005;
	for (unsigned int j = 0; j < e.height(); j++) {
		for (unsigned int i = 0; i < e.width(); i++) {
			e(i,j) = 0.0f;	
			r(i,j) = 0.0f;	
			
			if(SQUARE(i*e.dh() - e.realWidth()/2.0f) + SQUARE(j*e.dh() - e.realHeight()/2.0f) < r2)
				e(i,j) = 1.0f;	
		}
	}

	float alpha1=0.2f, alpha2=0.1f;
	float mu1=0.07, mu2=0.3;
	float kk=8.0f, epsilon=0.01, d=5e-5;
	
	std::map<std::string, Grid<float> *> map;
	map["e"] = &e;
	map["r"] = &r;
	SimpleSystem<float> system(map, epsilon, kk, d, mu1, mu2, alpha1, alpha2);
	Solver<float> solver;
	unsigned int nIterations = solver.solve(system, 100.0f);
	float dt = system.computeOptimalTimestep();

	
	makeGnuPlotScript("data/e.plot", "data/prefix_", "e", "_suffix.dat",0,nIterations,0,width,0,height,0.1,true,"data/e.gif");
	execGnuPlotScript("data/e.plot");
	showGif("data/e.gif");

	
	std::string prefix = "data/gpu_";
	std::string suffix = ".dat";
	
	cl_int err;
	log_console->infoStream() << "Listing platforms..";

	std::vector<cl::Platform> platforms;	
	std::vector<cl::Context> gpuContexts, cpuContexts, accContexts;
	std::vector<std::vector<cl::Device>> gpuDevices, cpuDevices, accDevices;

	unsigned int nPlatforms;
	unsigned int *nGpuDevices, *nCpuDevices, *nAccDevices;
	unsigned int nTotGpuDevices, nTotCpuDevices, nTotAccDevices;

	utils::loadDevicesAndCreateContexts(platforms,
										gpuContexts, cpuContexts, accContexts,
										gpuDevices, cpuDevices, accDevices,
										nPlatforms,
										nTotGpuDevices, nTotCpuDevices, nTotAccDevices,
										&nGpuDevices, &nCpuDevices, &nAccDevices);

	unsigned int nGpuContexts = gpuContexts.size();
	
	//Check devices
	if(nTotGpuDevices == 0) {
		log_console->critStream() << "No GPUs were found on your computer, aborting.";
		exit(EXIT_FAILURE);
	}
	if(nTotCpuDevices == 0) {
		log_console->warnStream() << "No CPUs were found on your computer.";
	}

	//Create a command queue for first gpu device from first gpu context
	unsigned int queuesPerDevice = 2;
	std::vector<std::vector<std::vector<cl::CommandQueue> > > commandQueues;
	for (unsigned int i = 0; i < nGpuContexts; i++) {
		std::vector<std::vector<cl::CommandQueue> > contextQueues;
		for (unsigned int j = 0; j < nGpuDevices[i]; j++) {
			std::vector<cl::CommandQueue> devicesQueues;
			for (unsigned int k = 0; k < queuesPerDevice; k++) {
				cl::CommandQueue queue(gpuContexts[i], gpuDevices[i][j]);
				devicesQueues.push_back(queue);
			}
			log_console->infoStream() << "Max arg bytes : " << gpuDevices[i][j].getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>();
			contextQueues.push_back(devicesQueues);
		}
		commandQueues.push_back(contextQueues);
	}


	//Load sources
	cl::Program::Sources sources = utils::loadSourcesFromFile("src/kernels/basicSystem.cl");	

	//Make program
	cl::Program program(gpuContexts[0], sources, &err); CHK_ERRORS(err);
	utils::buildProgram(program, gpuDevices[0], "", "Basic System Kernel");

	//Make kernel
	cl::Kernel kernel(program, "computeStep");
			
	log_console->infoStream() << "Max work group size  : " <<
	gpuDevices[0][0].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
	log_console->infoStream() << "Preferred work group size multiple : " << kernel.getWorkGroupInfo<CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE>(gpuDevices[0][0]);
	std::vector<size_t> maxItemSizes = gpuDevices[0][0].getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
	log_console->infoStream() << "Max work item sizes  : " << 
		maxItemSizes[0] << " x " << maxItemSizes[1] << " x " << maxItemSizes[2];
	log_console->infoStream() << "Global memory size  : " << utils::toStringMemory(gpuDevices[0][0].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>());
	log_console->infoStream() << "Local memory size  : " << utils::toStringMemory(gpuDevices[0][0].getInfo<CL_DEVICE_LOCAL_MEM_SIZE>());
	log_console->infoStream() << "Extensions : " << gpuDevices[0][0].getInfo<CL_DEVICE_EXTENSIONS>();

	//Buffers
	cl::Buffer E1_d(gpuContexts[0], CL_MEM_READ_WRITE, e.bytes());
	cl::Buffer E2_d(gpuContexts[0], CL_MEM_READ_WRITE, e.bytes());
	cl::Buffer R1_d(gpuContexts[0], CL_MEM_READ_WRITE, r.bytes());
	cl::Buffer R2_d(gpuContexts[0], CL_MEM_READ_WRITE, r.bytes());

	CHK_ERROR_RET(commandQueues[0][0][0].enqueueWriteBuffer(E1_d, CL_TRUE, 0, e.bytes(), e.data()));
	CHK_ERROR_RET(commandQueues[0][0][0].enqueueWriteBuffer(R1_d, CL_TRUE, 0, r.bytes(), r.data()));
	
	cl::NDRange global((e.width()+31)/32*32, (e.height()+31)/32*32);
	cl::NDRange local(32,32);

	log_console->infoStream() << "DT = " << dt;
	log_console->infoStream() << "DH = " << e.dh();

	char buffer[1000];
	for (unsigned int i = 0; i < nIterations; i++) {
		CHK_ERROR_RET(kernel.setArg<cl::Memory>(i%2, E1_d));
		CHK_ERROR_RET(kernel.setArg<cl::Memory>((i+1)%2, E2_d));
		CHK_ERROR_RET(kernel.setArg<cl::Memory>(2+i%2, R1_d));
		CHK_ERROR_RET(kernel.setArg<cl::Memory>(2+(i+1)%2, R2_d));
		CHK_ERROR_RET(kernel.setArg<float>(4, alpha1));
		CHK_ERROR_RET(kernel.setArg<float>(5, alpha2));
		CHK_ERROR_RET(kernel.setArg<float>(6, mu1));
		CHK_ERROR_RET(kernel.setArg<float>(7, mu2));
		CHK_ERROR_RET(kernel.setArg<float>(8, kk));
		CHK_ERROR_RET(kernel.setArg<float>(9, epsilon));
		CHK_ERROR_RET(kernel.setArg<float>(10, d));
		CHK_ERROR_RET(kernel.setArg<unsigned int>(11, width));
		CHK_ERROR_RET(kernel.setArg<unsigned int>(12, height));
		CHK_ERROR_RET(kernel.setArg<float>(13, dt));
		CHK_ERROR_RET(kernel.setArg<float>(14, e.dh()));

		CHK_ERROR_RET(commandQueues[0][0][0].enqueueNDRangeKernel(kernel, cl::NullRange, global, local));
		CHK_ERROR_RET(commandQueues[0][0][0].enqueueReadBuffer(i%2==0 ? E2_d : E1_d, CL_TRUE, 0, e.bytes(), e.data()));
		sprintf(buffer,"%se%i%s",prefix.c_str(),i,suffix.c_str());
		e.exportData(buffer);
	}

	makeGnuPlotScript("data/e2.plot", prefix, "e", suffix,0,nIterations,0,width,0,height,dt,true,"data/e2.gif");
	execGnuPlotScript("data/e2.plot");
	showGif("data/e2.gif");
	

	return EXIT_SUCCESS;
}

#endif
