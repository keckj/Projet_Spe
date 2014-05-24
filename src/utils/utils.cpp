#include "utils.hpp"

namespace utils {

	struct UserData{
		cl::Platform *platform;
		cl_device_type deviceType;
		unsigned int contextId;
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
			) 

	{

		CHK_ERROR_RET(cl::Platform::get(&platforms));
		nPlatforms = platforms.size();
		*nGpuDevices = new unsigned int[nPlatforms];
		*nCpuDevices = new unsigned int[nPlatforms];
		*nAccDevices = new unsigned int[nPlatforms];

		log_console.infoStream() << "\tFound " << nPlatforms << " platforms.";

		//Check devices and create contexts
		log_console.infoStream() << "Listing devices..";
		unsigned int i_gpu=0, i_cpu=0, i_acc=0;
		cl_int err;
		for (auto it = platforms.begin(); it != platforms.end(); ++it) {

			std::string name;
			it->getInfo(CL_PLATFORM_NAME, &name);
			log_console.infoStream() << "Switching to platform " << name;

			struct UserData **userData = new struct UserData*[3];
			for (int i = 0; i < 3; i++) userData[i] = new struct UserData;
			userData[0]->platform=&(*it); userData[0]->deviceType=CL_DEVICE_TYPE_GPU; userData[0]->contextId=i_gpu;
			userData[1]->platform=&(*it); userData[1]->deviceType=CL_DEVICE_TYPE_CPU; userData[1]->contextId=i_cpu;
			userData[2]->platform=&(*it); userData[2]->deviceType=CL_DEVICE_TYPE_ACCELERATOR; userData[2]->contextId=i_acc;

			cl_context_properties cps[3] = { 
				CL_CONTEXT_PLATFORM, 
				(cl_context_properties)(*it)(), 
				0 
			};

			cl::Context gpu_context(CL_DEVICE_TYPE_GPU, cps, openclContextCallback, userData[0], &err);
			if(err != CL_DEVICE_NOT_FOUND) {
				CHK_ERRORS(err);
				std::vector<cl::Device> gpu_devices = gpu_context.getInfo<CL_CONTEXT_DEVICES>(&err);
				CHK_ERRORS(err);
				if(gpu_devices.size() != 0) {
					log_console.infoStream() << "\tFound " << gpu_devices.size() << " GPUs.";
					nTotGpuDevices += gpu_devices.size();
					(*nGpuDevices)[i_gpu] = gpu_devices.size();
					gpuDevices.push_back(gpu_devices);
					gpuContexts.push_back(gpu_context);
					i_gpu++;
				}
			}

			cl::Context cpu_context(CL_DEVICE_TYPE_CPU, cps, openclContextCallback, userData[1], &err);
			if(err != CL_DEVICE_NOT_FOUND) {
				CHK_ERRORS(err);
				std::vector<cl::Device> cpu_devices = cpu_context.getInfo<CL_CONTEXT_DEVICES>(&err);
				CHK_ERRORS(err);
				if(cpu_devices.size() != 0) {
					log_console.infoStream() << "\tFound " << cpu_devices.size() << " CPUs.";
					nTotCpuDevices += cpu_devices.size();
					(*nCpuDevices)[i_cpu] = cpu_devices.size();
					cpuDevices.push_back(cpu_devices);
					cpuContexts.push_back(cpu_context);
					i_cpu++;
				}
			}

			cl::Context acc_context(CL_DEVICE_TYPE_ACCELERATOR, cps, openclContextCallback, userData[2], &err);
			if(err != CL_DEVICE_NOT_FOUND) {
				CHK_ERRORS(err);

				std::vector<cl::Device> acc_devices = acc_context.getInfo<CL_CONTEXT_DEVICES>(&err);
				CHK_ERRORS(err);
				if(acc_devices.size() != 0) {
					log_console.infoStream() << "\tFound " << acc_devices.size() << " accelerator devices.";
					nTotAccDevices += acc_devices.size();
					(*nAccDevices)[i_acc] = acc_devices.size();
					accDevices.push_back(acc_devices);
					accContexts.push_back(acc_context);
					i_acc++;
				}
			}
		}	
	}


	void openclContextCallback(const char *errorInfo, const void *privateInfoSize, size_t cb, void *userData) {
		struct UserData *data = (struct UserData *) userData;

		std::string platformName;
		data->platform->getInfo(CL_PLATFORM_NAME, &platformName);

		log_console.errorStream() << "OpenCL context callback :\n\t\t"
			<< "Context " << " (platform=" << platformName 
			<< ",device type="<<toStringDeviceType(data->deviceType)
			<< ",id=" << data->contextId
			<< ")\n\t\t" << errorInfo;

		exit(EXIT_FAILURE);
	}

	const std::string toStringDeviceType(cl_device_type deviceType) {
		switch(deviceType) {
			case(CL_DEVICE_TYPE_ALL): return "CL_DEVICE_TYPE_ALL"; break;
	    	  //case(CL_DEVICE_TYPE_CUSTOM): return "CL_DEVICE_TYPE_CUSTOM"; break;
			case(CL_DEVICE_TYPE_DEFAULT): return "CL_DEVICE_TYPE_DEFAULT"; break;
			case(CL_DEVICE_TYPE_CPU): return "CL_DEVICE_TYPE_CPU"; break;
			case(CL_DEVICE_TYPE_GPU): return "CL_DEVICE_TYPE_GPU"; break;
			case(CL_DEVICE_TYPE_ACCELERATOR): return "CL_DEVICE_TYPE_ACCELERATOR"; break;
			default: return "Unknown OpenCL Device Type";
		}
	}
}

