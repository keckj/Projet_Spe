
#ifndef DEVICETHREAD_H
#define DEVICETHREAD_H

#include "headers.hpp"
#include "subDomain.hpp"
#include "fence.hpp"
#include "argument.hpp"

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
				Fence *fence,
				std::map<QString, Argument> *args,
				float dh,
				bool display=true,
				bool writeOnDisk=false,
				const std::string &rep="");

			~DeviceThread();

		void operator()();

	private:
		void initSubDomain(std::map<std::string, MultiBufferedSubDomain<float, 1u>*> subDomain);
		void computeSubDomainStep(unsigned int domainId, unsigned int stepId);
		void finishCommandQueues();
		
		float computeOptimalTimestep();

		MultiGpu *_simulation;

		const cl::Platform _platform;
		const cl::Context _context;
		const cl::Program _program;
		const cl::Device _device;
		cl::Kernel _kernel;

		Fence *_fence;

		float _dh, _dt;
		float _epsilon, _kk, _dd;
		float _mu1, _mu2;
		float _alpha1, _alpha2;

		bool _display;
		bool _writeOnDisk;

		cl::CommandQueue _commandQueues[nCommandQueues];

		std::vector<std::map<std::string, MultiBufferedSubDomain<float,1u>*>> _acquiredDomains; 

		std::vector<std::map<std::string, std::vector<cl::Buffer>>> _acquiredDomainsVarBuffers;
		std::vector<std::map<std::string, std::vector<cl::Buffer>>> _acquiredDomainsInternalEdgeBuffers;
		std::vector<std::map<std::string, std::vector<cl::Buffer>>> _acquiredDomainsExternalEdgeBuffers;
		std::vector<std::map<std::string, float* const**>> _acquiredDomainsInternalEdgeHostData;
		std::vector<std::map<std::string, float***>> _acquiredDomainsExternalEdgeHostData;
		
		std::vector<bool> _acquiredDomainsIsInitialDataSent;

		static std::mutex _mutex;
		static std::condition_variable _cond;
		static bool _called;

		template <typename Ret, class... Args>
		static void callOnce(std::function<Ret(Args...)> f, Fence *fence, Args... args);
};
		
template <unsigned int nCommandQueues>
template <typename Ret, class... Args>
void DeviceThread<nCommandQueues>::callOnce(std::function<Ret(Args...)> f, Fence *fence, Args... args) {
	_called=false;
	(*fence)();
	{
		std::unique_lock<std::mutex> lock(DeviceThread::_mutex);
		if(!_called) {
			f(args...);		
			_called = true;
		}
		else {
		}
		DeviceThread::_cond.notify_one();
	}
	(*fence)();
}

#endif /* end of include guard: DEVICETHREAD_H */
