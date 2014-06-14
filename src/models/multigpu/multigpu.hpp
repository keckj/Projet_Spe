
#ifndef MULTIGPU_H
#define MULTIGPU_H

#include "deviceThread.hpp"
#include "headers.hpp"
#include "fence.hpp"

#include "model.hpp"
#include "domain.hpp"
#include "initialCond.hpp"

#include <mutex>
#include <condition_variable>

class MultiGpu : public Model {
	Q_OBJECT
    
	public:
		MultiGpu(int nbIter, 
				unsigned int width_, unsigned int height_, unsigned int length_,
				std::map<QString, Argument> *args, 
				std::map<QString, bool> *renderedVars);
        ~MultiGpu();
		void killWorkerThreads();

		//Model overrides
        void initComputation() override;
        void computeStep(int i) override;
        void finishComputation() override;
		bool _step;
		bool _stepDone;
	
		//acessors for slicesIds and generated textures
		unsigned int sliceIdX();
		unsigned int sliceIdY();
		unsigned int sliceIdZ();
		std::map<std::string, float*> sliceX();
		std::map<std::string, float*> sliceY();
		std::map<std::string, float*> sliceZ();
		
		//thread safe acessors for worker threads
		bool subDomainAvailable();
		bool tryToTakeSubDomain(std::map<std::string, MultiBufferedSubDomain<float,1u>*> &subdomain);
		void releaseSubDomain(std::map<std::string, MultiBufferedSubDomain<float,1u>*> subDomain);
		void resetSubDomains();
		
		//signals from worker threads
		void initDone();
		void stepDone();
		void lockStep();
		void giveStep();
		void releaseStep();
		void waitStep();
		void waitCompute();
		void waitInit();
		void destroy();
		void abort();
	
		//access for gui
		static std::map<QString, Argument> *getArguments();
        static std::map<QString, bool> *getVariables();

	private:
		//openCL context
		cl::Platform _platform;
		cl::Context _context;
		std::vector<cl::Device> _devices;
		std::vector<DeviceThread<1u>*> _deviceThreads;
		unsigned int _nDevices;
		void initOpenClContext();
		
		//thread safety
		std::mutex _mutex, _domainMutex;
		std::condition_variable _cond, _domainCond;
		std::list<unsigned int> _availableDomains;

		//domain
		unsigned int _nFunctions;
		std::map<std::string, MultiBufferedDomain<float,1u>*> _domains;
		unsigned int _splits;
		bool _init;
		void initGrids(const std::map<std::string, InitialCond<float>*> &initialConditions);

		//draw & save on disk
		unsigned int _sliceIdX, _sliceIdY, _sliceIdZ;
		std::map<std::string, float*> _sliceX, _sliceY, _sliceZ;
		void allocSlices();
		void createTextures();
		void renderToTextures();
	
		//model
		std::map<QString, Argument> *_args;
		bool _kill, _destroyed;
};

#include "deviceThread.tpp"

#endif /* end of include guard: MULTIGPU_H */
