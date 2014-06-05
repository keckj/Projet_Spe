
#include "headers.hpp"
#include "multigpu.hpp"
#include "multigpu.moc"
#include "log.hpp"
#include "domain.hpp"

#include <sstream>

using namespace log4cpp;
using namespace utils;

MultiGpu::MultiGpu(int nbIter) : 
	Model(10),
	_nDevices(0),
	_nFunctions(0),
	_initialCondGrids(0),
	_gridWidth(0), _gridHeight(0), _gridLength(0),
	_subGridWidth(0), _subGridHeight(0), _subGridLength(0)
{
}

MultiGpu::~MultiGpu() 
{
}

void MultiGpu::initComputation() {
	initGrids(NULL);
	checkGrids();

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
		

void MultiGpu::initGrids(std::map<std::string, Grid<float>*> *initialCondGrids) {
		_initialCondGrids = new std::map<std::string, Grid<float>*>;
		
		Grid<float> *gridE = new Grid3D<float>(512u,512u,512u,0.01f);
		Grid<float> *gridR = new Grid3D<float>(512u,512u,512u,0.01f);

		(*_initialCondGrids)["e"] = gridE;
		(*_initialCondGrids)["r"] = gridR;
		
		_gridWidth = 1233u;
		_gridHeight = 457u;
		_gridLength = 789u;
		_nFunctions = _initialCondGrids->size();

		Domain dom(_gridWidth, _gridHeight, _gridLength, 1u, 17);
}

void MultiGpu::checkGrids() {
	for(auto grid = _initialCondGrids->begin(); grid != _initialCondGrids->end(); ++grid) {
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

