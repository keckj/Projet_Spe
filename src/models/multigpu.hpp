
#ifndef MULTIGPU_H
#define MULTIGPU_H

#include "headers.hpp"

#include "utils/headers.hpp"
#include "model.hpp"

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
		unsigned int _nDevices;
		std::mutex mutex_t;
		std::condition_variable cond_t;

		unsigned int _nFunctions;
		std::map<std::string, Grid<float>*> *_initialCondGrids;
		
		unsigned int _gridWidth, _gridHeight, _gridLength;
		unsigned int *_subGridWidth, *_subGridHeight, *_subGridLength;
	
		unsigned int _order;
		float *gridEdgesX, *gridEdgesY, *gridEdgesZ; 

		void initOpenClContext();
		void createGlObjects();
		
		void initGrids(std::map<std::string, Grid<float>*> *initialCondGrids);
		void checkGrids();


    signals:
        void stepComputed(const Grid2D<float> *grid);
        void stepComputed(const Grid3D<float> *grid);
        void finished();
};


#endif /* end of include guard: MULTIGPU_H */
