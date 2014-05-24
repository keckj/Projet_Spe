
#ifndef UTILS_H

#include "headers.hpp"
#include <vector>

namespace utils {

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


	void openclContextCallback(const char *errorInfo, const void *privateInfoSize, size_t cb, void *userData);
	
	const std::string toStringDeviceType(cl_device_type deviceType);
}


#endif /* end of include guard: UTILS_H */
