
#ifndef MULTIGPU_H
#define MULTIGPU_H

#include "headers.hpp"

#include "utils/headers.hpp"
#include "model.hpp"

#include <mutex>

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
		std::mutex mutex;

		unsigned int _nFunctions;
		std::map<std::string, Grid<float>*> *_grids;
		std::map<std::string, Grid<float>*> *_gridsOld;
		
		unsigned int _nSubGrids;
		unsigned int _splitX, _splitY, _splitZ;
		unsigned int _gridWidth, _gridHeight, _gridLength;
		unsigned int *_subGridWidth, *_subGridHeight, *_subGridLength;

		void initOpenClContext();
		void createGlObjects();
		
		void initGrids();
		void checkGrids();
		void splitProblem(unsigned int minSplit);
    
    signals:
        void stepComputed(const Grid2D<float> *grid);
        void stepComputed(const Grid3D<float> *grid);
        void finished();
};

#endif /* end of include guard: MULTIGPU_H */
