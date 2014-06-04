
#include "headers.hpp"
#include "multigpu.hpp"
#include "multigpu.moc"
#include "log.hpp"

#include <sstream>

using namespace log4cpp;
using namespace utils;

MultiGpu::MultiGpu(int nbIter) : 
	Model(10),
	_nDevices(0),
	_nFunctions(0),
	_grids(0),
	_gridsOld(0),
	_gridWidth(0), _gridHeight(0), _gridLength(0),
	_subGridWidth(0), _subGridHeight(0), _subGridLength(0)
{
}

MultiGpu::~MultiGpu() 
{
}

void MultiGpu::initComputation() {
	initGrids();
	checkGrids();
	splitProblem(4);
	initOpenClContext();
}

void MultiGpu::computeStep(int i) {;}
void MultiGpu::finishComputation() {;}

void MultiGpu::initOpenClContext(){
	
	//look for compatible platforms
	std::vector<cl::Platform> platforms;
	CHK_ERROR_RET(cl::Platform::get(&platforms));
	
	std::vector<cl::Device> devices;
	bool found = false;

	for(auto it : platforms) {
		std::string name = it.getInfo<CL_PLATFORM_NAME>();
		std::string extensions = it.getInfo<CL_PLATFORM_EXTENSIONS>();
		
		//check platforms extensions
		std::stringstream ss(extensions);
		std::string ext;
		while(ss) {
			ss >> ext;
			if (ext.compare(CL_GL_SHARING_EXT) == 0) {
				log_console->infoStream() << "Platform " << name << " has openGL/openCL sharing capabilities :";
			
				//if platform supported, look for supported devices
				CHK_ERROR_RET(it.getDevices(CL_DEVICE_TYPE_GPU, &devices));
				for(auto dev : devices) {
					std::string devExtensions = dev.getInfo<CL_DEVICE_EXTENSIONS>();
					std::string devName = dev.getInfo<CL_DEVICE_NAME>();
					std::stringstream ss2(extensions);
					std::string devExt;
					bool deviceSupported = false;
					while(ss2) {
						ss >> devExt;
						if (ext.compare(CL_GL_SHARING_EXT) == 0) {
							deviceSupported = true;
							break;
						}
					}
					if (deviceSupported) {
							log_console->infoStream() << "\tDevice " << devName << ": supported";
							_devices.push_back(dev);
					}
					else {
							log_console->infoStream() << "\tDevice " << devName << ": NOT supported";
					}
				}

				_platform = it;
				found = true;
				break;
			}
		}
	}
	
	_nDevices = _devices.size();

	if(!found) {
			log_console->infoStream() << "No platform has openGL/openCL sharing capabilities, aborting !";
			emit(finished());
	}
	if(_nDevices == 0) {
			log_console->infoStream() << "A platform has openGL/openCL sharing capabilities, but no supported devices found !";
			emit(finished());
	}

	//Create context for supported devices
	cl_context_properties contextProperties[] = { 
					CL_CONTEXT_PLATFORM, (cl_context_properties) _platform(), 
					CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
					CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(), 
					0 
	};
			
	struct ContextUserData *callbackData = new struct ContextUserData;
	*callbackData = {&_platform, CL_DEVICE_TYPE_GPU,0};

	cl_int err;
	
	_context = cl::Context(_devices, contextProperties, openclContextCallback, (void*)callbackData, &err);
	if(err == CL_INVALID_OPERATION) {
		log_console->warnStream() << "Impossible to create a shared context, switching to a normal openCL context.";
		contextProperties[2] = (cl_context_properties) 0; 
		_context = cl::Context(_devices, contextProperties, openclContextCallback, (void*)callbackData, &err);
	}
	CHK_ERRORS(err);
}
void MultiGpu::createGlObjects(){}
		

void MultiGpu::initGrids() {
		_grids = new std::map<std::string, Grid<float>*>;
		_gridsOld = new std::map<std::string, Grid<float>*>;
		
		Grid<float> *gridE_A = new Grid3D<float>(512u,512u,512u,0.01f);
		Grid<float> *gridE_B = new Grid3D<float>(512u,512u,512u,0.01f);

		Grid<float> *gridR_A = new Grid3D<float>(512u,512u,512u,0.01f);
		Grid<float> *gridR_B = new Grid3D<float>(512u,512u,512u,0.01f);

		(*_grids)["e"] = gridE_A;
		(*_grids)["r"] = gridR_A;
		
		(*_gridsOld)["e"] = gridE_B;
		(*_gridsOld)["r"] = gridR_B;

		_gridWidth = 512u;
		_gridHeight = 512u;
		_gridLength = 512u;
}

void MultiGpu::checkGrids() {
	for(auto grid = _grids->begin(); grid != _grids->end(); ++grid) {
		if(!grid->second->isAllocated()) {
			log_console->errorStream() << "Grid '" << grid->first << "' is not allocated, aborting.";
			emit(finished());
		}
		if(grid->second->width() != _gridWidth
			|| grid->second->height() != _gridHeight
			|| grid->second->length() != _gridLength) {
			log_console->errorStream() << "Grid sizes are not coherent, aborting.";
			emit(finished());
		}
		if(!grid->second->isOwner()) {
			log_console->warnStream() << "Grid '" << grid->first << "' is allocated but is not data owner.";
		}
	}
	
	for(auto grid = _gridsOld->begin(); grid != _gridsOld->end(); ++grid) {
		if(!grid->second->isAllocated()) {
			log_console->errorStream() << "Grid '" << grid->first << "' is not allocated, aborting.";
			emit(finished());
		}
		if(grid->second->width() != _gridWidth
			|| grid->second->height() != _gridHeight
			|| grid->second->length() != _gridLength) {
			log_console->errorStream() << "Grid sizes are not coherent, aborting.";
			emit(finished());
		}
		if(!grid->second->isOwner()) {
			log_console->warnStream() << "Grid '" << grid->first << "' is allocated but is not data owner.";
		}
	}
}

void MultiGpu::splitProblem(unsigned int minSplit) {
		
	unsigned int nSplit = minSplit;

	unsigned int splitX=1, splitY=1, splitZ=1;
	unsigned int subGridWidth=_gridWidth, subGridHeight=_gridHeight, subGridLength=_gridLength;
	
	while(splitX*splitY*splitZ < nSplit) {
		if(subGridWidth > subGridHeight && subGridWidth > subGridLength) { //on privilégie la découpe en z puis y puis x
			subGridWidth = (_gridWidth+splitX-1)/splitX;
			splitX++;
		}
		else {
			if(subGridHeight > subGridLength) {
				subGridHeight = (_gridHeight+splitY-1)/splitY;
				splitY++;
			}
			else {
				subGridLength = (_gridLength+splitZ-1)/splitZ;
				splitZ++;
			}
		}
	}
	
	_splitX = splitX;
	_splitY = splitY;
	_splitZ = splitZ;
	_nSubGrids = _splitX*_splitY*_splitZ;

	_subGridWidth = new unsigned int[_nSubGrids];
	_subGridHeight = new unsigned int[_nSubGrids];
	_subGridLength = new unsigned int[_nSubGrids];

	for (unsigned int k = 0; k < splitZ; k++) {
		for (unsigned int j = 0; j < splitY; j++) {
			for (unsigned int i = 0; i < splitX; i++) {
				_subGridWidth [k*splitY*splitX + j*splitX + i] = (i==splitX-1 ? _gridWidth %splitX : _gridWidth /splitX);
				_subGridHeight[k*splitY*splitX + j*splitX + i] = (j==splitY-1 ? _gridHeight%splitY : _gridHeight/splitY);
				_subGridLength[k*splitY*splitX + j*splitX + i] = (k==splitZ-1 ? _gridLength%splitZ : _gridLength/splitZ);
			}
		}
	}

	_nSubGrids = 1 << nSplit;
}
