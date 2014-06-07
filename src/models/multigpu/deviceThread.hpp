
#ifndef DEVICETHREAD_H
#define DEVICETHREAD_H

#include "headers.hpp"
#include "subDomain.hpp"
#include "fence.hpp"

#include <functional>

class MultiGpu;

template <unsigned int nCommandQueues = 2u>
class DeviceThread {

	public:
		DeviceThread(MultiGpu *simulation, 
				const cl::Platform &platform,
				const cl::Context &context,
				const cl::Program &program,
				const cl::Device &device,
				Fence *fence);

		~DeviceThread();
		
		void operator()();

	private:
		void init();

		MultiGpu *_simulation;

		const cl::Platform _platform;
		const cl::Context _context;
		const cl::Program _program;
		const cl::Device _device;

		Fence *_fence;

		cl::CommandQueue _commandQueues[nCommandQueues];

		std::map<std::string, MultiBufferedSubDomain<float,1u>*> _currentDomain; 
		
		static std::mutex _mutex;
		static std::condition_variable _cond;
		static bool _called;

		template <typename Ret, class... Args>
		static void callOnce(std::function<Ret(Args...)> f, Fence *fence, Args... args);
};
		
template <unsigned int nCommandQueues>
template <typename Ret, class... Args>
void DeviceThread<nCommandQueues>::callOnce(std::function<Ret(Args...)> f, Fence *fence, Args... args) {
	(*fence)();
	_called = false;
	{
		std::unique_lock<std::mutex> lock(DeviceThread::_mutex);
		if(!_called) {
			f(args...);		
			_called = true;
		}
		_cond.notify_one();
	}
	(*fence)();
}

#endif /* end of include guard: DEVICETHREAD_H */
