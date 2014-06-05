
#ifndef MULTIGPU_H
#define MULTIGPU_H

#include "headers.hpp"

#include "utils/headers.hpp"
#include "model.hpp"

#include "domain.hpp"
#include "initialCond.hpp"
#include "deviceThread.hpp"

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

	private:
		cl::Platform _platform;
		cl::Context _context;
		std::vector<cl::Device> _devices;
		std::vector<DeviceThread<>> _deviceThreads;
		unsigned int _nDevices;
		
		std::mutex _mutex;
		std::condition_variable _cond;

		unsigned int _nFunctions;
		unsigned int _gridWidth, _gridHeight, _gridLength;
		std::map<std::string, MultiBufferedDomain<float,2u>> _domains;
		
		void initOpenClContext();
		void createGlObjects();
		
		void initGrids(const std::map<std::string, InitialCond<float>*> &initialConditions);

		const cl::Platform &platform();
		const cl::Context &context();
		std::map<std::string, MultiBufferedSubDomain<float,2u>*> takeSubDomain();
		std::map<std::string, MultiBufferedSubDomain<float,2u>*> releaseSubDomain();


    signals:
        void stepComputed(const Grid2D<float> *grid);
        void stepComputed(const Grid3D<float> *grid);
        void finished();
};


#endif /* end of include guard: MULTIGPU_H */
