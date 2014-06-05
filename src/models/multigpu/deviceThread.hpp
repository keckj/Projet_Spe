

#ifndef DEVICETHREAD_H
#define DEVICETHREAD_H

#include "headers.hpp"
#include "multigpu.hpp"


class MultiGpu;

template <unsigned int nCommandQueues = 2u>
class DeviceThread {

	public:
		DeviceThread(MultiGpu *simulation, 
				const cl::Platform &platform,
				const cl::Context &context,
				const cl::Device &device);

		~DeviceThread();

	private:
		void takeSubDomain();
		void releaseSubDomain();

		MultiGpu *_simulation;

		const cl::Platform _platform;
		const cl::Context _context;
		const cl::Device _device;

		cl::CommandQueue _commandQueues[nCommandQueues];
};

#include "deviceThread.tpp"

#endif /* end of include guard: DEVICETHREAD_H */
