
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
        MultiGpu(int nbIter);
        ~MultiGpu();

        void initComputation() override;
        void computeStep(int i) override;
        void finishComputation() override;
		
		bool subDomainAvailable();
		bool tryToTakeSubDomain(std::map<std::string, MultiBufferedSubDomain<float,1u>*> &subdomain);
		void releaseSubDomain(std::map<std::string, MultiBufferedSubDomain<float,1u>*> subDomain);
		void resetSubDomains();

		void initDone();
		void stepDone();

	private:
		cl::Platform _platform;
		cl::Context _context;
		std::vector<cl::Device> _devices;
		std::vector<DeviceThread<1u>> _deviceThreads;
		unsigned int _nDevices;
		
		std::mutex _mutex;
		std::condition_variable _cond;
		std::list<unsigned int> _availableDomains;

		unsigned int _nFunctions;
		unsigned int _gridWidth, _gridHeight, _gridLength;
		std::map<std::string, MultiBufferedDomain<float,1u>*> _domains;
		unsigned int _splits;
		bool _init;

		Grid<float> *_grid;
		
		void initOpenClContext();
		
		void initGrids(const std::map<std::string, InitialCond<float>*> &initialConditions);
		

    signals:
        void stepComputed(const Grid2D<float> *grid);
        void stepComputed(const Grid3D<float> *grid);
        void finished();
};

#include "deviceThread.tpp"


#endif /* end of include guard: MULTIGPU_H */
