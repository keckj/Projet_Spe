
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

	template<typename A, typename B>
		std::pair<B,A> flip_pair(const std::pair<A,B> &p)
		{
			return std::pair<B,A>(p.second, p.first);
		}

	template<typename A, typename B>
		std::map<B,A> flip_map(const std::map<A,B> &src)
		{
			std::map<B,A> dst;
			std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), 
					flip_pair<A,B>);
			return dst;
		}

	template<typename A, typename B>
		std::vector<std::pair<B,A> > mapToReversePairVector(const std::map<A,B> &src) {
			std::vector<std::pair<B,A> > dst;

			auto it = src.begin();
			while(it != src.end()) {
				dst.push_back(std::pair<B,A>(it->second, it->first));
				++it;
			}

			return dst;
		}
}

#endif /* end of include guard: UTILS_H */
