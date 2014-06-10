
#ifndef CLUTILS_H
#define CLUTILS_H

#include "headers.hpp"
#include <vector>
#include <limits>

namespace utils {
	
	struct ContextUserData{
		cl::Platform *platform;
		cl_device_type deviceType;
		unsigned int contextId;
	};

	struct BuildUserData{
		cl::Program *program;
		std::string programName;
	};



	void loadDevicesAndCreateContexts(
			std::vector<cl::Platform> &platforms,
			std::vector<cl::Context> &gpuContexts, 
			std::vector<cl::Context> &cpuContexts, 
			std::vector<cl::Context> &accContexts, 
			std::vector<std::vector<cl::Device>> &gpuDevices,
			std::vector<std::vector<cl::Device>> &cpuDevices,
			std::vector<std::vector<cl::Device>> &accDevices,
			unsigned int &nPlatforms,
			unsigned int &nTotGpuDevices, 
			unsigned int &nTotCpuDevices, 
			unsigned int &nTotAccDevices,
			unsigned int **nGpuDevices,
			unsigned int **nCpuDevices,
			unsigned int **nAccDevices
			);


	cl::Program::Sources loadSourcesFromFile(const std::string &srcFile);
	void buildProgram(cl::Program &program, std::vector<cl::Device> &targetDevices, const char *buildOptions, const std::string &programName);

	void openclContextCallback(const char *errorInfo, const void *privateInfoSize, size_t cb, void *userData);
	void openclBuildCallback(cl_program program, void *user_data);
	
	const std::string toStringDeviceType(cl_device_type deviceType);
	const std::string toStringBuildStatus(cl_build_status buildStatus);

	void clAssert(cl_int err, const std::string &file, int line, bool abort = true);
}


#endif /* end of include guard: CLUTILS_H */
