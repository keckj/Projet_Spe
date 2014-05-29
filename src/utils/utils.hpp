
#ifndef UTILS_H
#define UTILS_H
	
#define SQUARE(X) ((X)*(X))

#include "headers.hpp"
#include <vector>
#include <limits>

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


	cl::Program::Sources loadSourcesFromFile(const std::string &srcFile);
	void buildProgram(cl::Program &program, std::vector<cl::Device> &targetDevices, const char *buildOptions, const std::string &programName);

	void openclContextCallback(const char *errorInfo, const void *privateInfoSize, size_t cb, void *userData);
	void openclBuildCallback(cl_program program, void *user_data);
	
	const std::string toStringDeviceType(cl_device_type deviceType);
	const std::string toStringBuildStatus(cl_build_status buildStatus);
	const std::string toStringMemory(unsigned long bytes);
	
	template <typename T>
	T abs(T val) {
		return (val < T(0) ? -val : val);
	}
	
	template <typename T>
	bool areEqual(T val1, T val2) {
		return (abs<T>(val2 - val1) <= std::numeric_limits<T>::epsilon() * std::max(abs<T>(val1), abs<T>(val2)));
	}
}

#endif /* end of include guard: UTILS_H */
