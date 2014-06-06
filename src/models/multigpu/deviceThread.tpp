
#include <thread>
#include "log.hpp"

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
				const cl::Device &device,
				Fence *fence) :
	_simulation(simulation), 
	_platform(platform), _context(context), _device(device),
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

	(*_fence)();

	while(_simulation->subDomainAvailable()) {
		bool success = _simulation->tryToTakeSubDomain(_currentDomain);
		if(success)
			this->init();
	}

	std::function<void(MultiGpu*)> f(&MultiGpu::initDone);
	callOnce<void,MultiGpu*>(f, _fence, _simulation);
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
