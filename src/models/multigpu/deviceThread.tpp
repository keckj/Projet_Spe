
#include "headers.hpp"
#include "log.hpp"
#include "clUtils.hpp"
#include <thread>

using namespace log4cpp;
		
template <unsigned int nCommandQueues>
std::mutex DeviceThread<nCommandQueues>::_mutex;

template <unsigned int nCommandQueues>
std::condition_variable DeviceThread<nCommandQueues>::_cond;

template <unsigned int nCommandQueues>
bool DeviceThread<nCommandQueues>::_called;
	
template <unsigned int nCommandQueues>
DeviceThread<nCommandQueues>::DeviceThread(MultiGpu *simulation, 
				const cl::Platform &platform,
				const cl::Context &context,
				const cl::Program &program,
				const cl::Device &device,
				Fence *fence) :
	_simulation(simulation), 
	_platform(platform), _context(context),
	_program(program), _device(device),
	_fence(fence)
{
}

template <unsigned int nCommandQueues>
DeviceThread<nCommandQueues>::~DeviceThread() {
}

template <unsigned int nCommandQueues>
void DeviceThread<nCommandQueues>::operator()() {
	cl_int err;
	for (int i = 0; i < nCommandQueues; i++) {
		_commandQueues[i] = cl::CommandQueue(_context, _device, CL_QUEUE_PROFILING_ENABLE, &err); 
		CHK_ERRORS(err);
	}
	
	float dt = float(0.138);
	float dh = float(0.01);
	float alpha1 = float(0.2);
	float alpha2 = float(0.1);
	float mu1 = float(0.07);
	float mu2 = float(0.3);
	float epsilon = float(0.01);
	float dd = float(5e-5);
	float kk = float(8);

	(*_fence)();

	while(_simulation->subDomainAvailable()) {
		bool success = _simulation->tryToTakeSubDomain(_currentDomain);
		if(success)
			this->init();
	}

	std::function<void(MultiGpu*)> f(&MultiGpu::initDone);
	callOnce<void,MultiGpu*>(f, _fence, _simulation);

	//Make kernel
	cl::Kernel kernel(_program, "computeStep");
	
	//Each threads get their subdomains
	std::map<std::string, std::vector<cl::Buffer>> varBuffers;
	
	//Prepare
	bool success = _simulation->tryToTakeSubDomain(_currentDomain);
	if(success) {
		
		unsigned int subDomainWidth = _currentDomain.begin()->second->subDomainWidth();
		unsigned int subDomainHeight = _currentDomain.begin()->second->subDomainHeight();
		unsigned int subDomainLength = _currentDomain.begin()->second->subDomainLength();
		unsigned long bytes = _currentDomain.begin()->second->bytes();

		for(auto pairs : _currentDomain) {
			std::vector<cl::Buffer> varBuffer;
			cl::Buffer B1_d(_context, CL_MEM_READ_WRITE, bytes);
			cl::Buffer B2_d(_context, CL_MEM_READ_WRITE, bytes);
			varBuffer.push_back(B1_d);
			varBuffer.push_back(B2_d);
			varBuffers.emplace(pairs.first, varBuffer);
		}
	
		//COMPUTE
		
		//Send func data
		for(auto pair : _currentDomain) {
			CHK_ERROR_RET(_commandQueues[0].enqueueWriteBuffer(varBuffers[pair.first][0], CL_TRUE, 0, bytes, *(pair.second->data())));
			log_console->debugStream() << "Sending '" << pair.first << "' initial variable data from subdomain " << pair.second->id() << " to GPU !";
		}
		
		cl::NDRange global((subDomainWidth+31)/32*32, (subDomainHeight+31)/32*32);
		cl::NDRange local(32,32);

		for (unsigned int i = 0; i < 1000; i++) {
			CHK_ERROR_RET(kernel.setArg<cl::Memory>(i%2, varBuffers["e"][0]));
			CHK_ERROR_RET(kernel.setArg<cl::Memory>((i+1)%2, varBuffers["e"][1]));
			CHK_ERROR_RET(kernel.setArg<cl::Memory>(2+i%2,varBuffers["r"][0]));
			CHK_ERROR_RET(kernel.setArg<cl::Memory>(2+(i+1)%2, varBuffers["r"][1]));
			CHK_ERROR_RET(kernel.setArg<float>(4, alpha1));
			CHK_ERROR_RET(kernel.setArg<float>(5, alpha2));
			CHK_ERROR_RET(kernel.setArg<float>(6, mu1));
			CHK_ERROR_RET(kernel.setArg<float>(7, mu2));
			CHK_ERROR_RET(kernel.setArg<float>(8, kk));
			CHK_ERROR_RET(kernel.setArg<float>(9, epsilon));
			CHK_ERROR_RET(kernel.setArg<float>(10, dd));
			CHK_ERROR_RET(kernel.setArg<unsigned int>(11, subDomainWidth));
			CHK_ERROR_RET(kernel.setArg<unsigned int>(12, subDomainHeight));
			CHK_ERROR_RET(kernel.setArg<float>(13, dh));
			CHK_ERROR_RET(kernel.setArg<float>(14, dt));

			CHK_ERROR_RET(_commandQueues[0].enqueueNDRangeKernel(kernel, cl::NullRange, global, local));
			_commandQueues[0].enqueueReadBuffer(varBuffers["e"][(i+1)%2], CL_TRUE, 0, bytes, *(_currentDomain["e"]->data()));

			CHK_ERROR_RET(clFinish(_commandQueues[0]()));
			log_console->debugStream() << "Fuckin' VARBUFFERS : " << "\t" << varBuffers["e"][0]() << "\t" << varBuffers["e"][1]() << "\t" << varBuffers["r"][0]() << "\t" << varBuffers["r"][1]() << " ...";
			log_console->debugStream() << "Domain " << _currentDomain["e"]->id() << " " << *(_currentDomain["e"]->data()) << " done !";
			std::function<void(MultiGpu*)> g(&MultiGpu::stepDone);
			callOnce<void,MultiGpu*>(g, _fence, _simulation);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	else {
		log_console->infoStream() << "Device " << _device() << " is fapping !";
	}


	(*_fence)();

}

template <unsigned int nCommandQueues>
void DeviceThread<nCommandQueues>::init() {
	log_console->debugStream() << "Thread " << std::this_thread::get_id() 
		<< " took subdomain " << _currentDomain.begin()->second->id()
		<< " for initialization !";

	for(auto &func : _currentDomain) {
		func.second->initSubDomain(0);	
	}
}
