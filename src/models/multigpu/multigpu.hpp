
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
	
		//device thread funcs
		bool subDomainAvailable();
		bool tryToTakeSubDomain(std::map<std::string, MultiBufferedSubDomain<float,1u>*> &subdomain);
		void releaseSubDomain(std::map<std::string, MultiBufferedSubDomain<float,1u>*> subDomain);
		void resetSubDomains();

		void initDone();
		void stepDone();

		unsigned int sliceIdX();
		unsigned int sliceIdY();
		unsigned int sliceIdZ();
		std::map<std::string, float*> sliceX();
		std::map<std::string, float*> sliceY();
		std::map<std::string, float*> sliceZ();

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
		unsigned int _sliceIdX, _sliceIdY, _sliceIdZ;
		std::map<std::string, float*> _sliceX, _sliceY, _sliceZ;

		Display *_display;
		GLXContext _ctx;
		Window _win;
		Colormap _cmap;

		std::map<std::string, unsigned int> _mapped_textures;
		
		void initOpenClContext();
		
		void initGrids(const std::map<std::string, InitialCond<float>*> &initialConditions);

		void createOpenGLContext(Display **display, GLXContext *ctx, Window *win, Colormap *cmap);
		void allocSlices();
		void createTextures();
		void renderToTextures();

		static int contextErrorHandler(Display *dpy, XErrorEvent *ev);
		static bool _contextError;

		

    signals:
        void stepComputed(const Grid2D<float> *grid);
        void stepComputed(const Grid3D<float> *grid);
        void finished();
};

#include "deviceThread.tpp"

#endif /* end of include guard: MULTIGPU_H */
