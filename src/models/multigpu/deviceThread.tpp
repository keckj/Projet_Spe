
		
template <unsigned int nCommandQueues>
DeviceThread<nCommandQueues>::DeviceThread(MultiGpu *simulation, 
				const cl::Platform &platform,
				const cl::Context &context,
				const cl::Device &device) :
	_simulation(simulation), 
	_platform(platform), _context(context), _device(device)
{
	cl_int err;
	for (int i = 0; i < nCommandQueues; i++) {
		_commandQueues[i] = cl::CommandQueue(_context, _device, CL_QUEUE_PROFILING_ENABLE, &err); 
		CHK_ERRORS(err);
	}
}

template <unsigned int nCommandQueues>
DeviceThread<nCommandQueues>::~DeviceThread() {}

template <unsigned int nCommandQueues>
void DeviceThread<nCommandQueues>::takeSubDomain() {
}

template <unsigned int nCommandQueues>
void DeviceThread<nCommandQueues>::releaseSubDomain() {
}
