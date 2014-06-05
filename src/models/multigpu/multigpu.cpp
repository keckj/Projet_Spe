
#include "headers.hpp"
#include "multigpu.hpp"
#include "multigpu.moc"
#include "log.hpp"
#include "domain.hpp"

#include "circleInitialCond.hpp"
#include "functionInitialCond.hpp"

#include <sstream>

using namespace log4cpp;
using namespace utils;

MultiGpu::MultiGpu(int nbIter) : 
	Model(10),
	_nDevices(0),
	_nFunctions(0),
	_gridWidth(0), _gridHeight(0), _gridLength(0)
{
}

MultiGpu::~MultiGpu() 
{
}

void MultiGpu::initComputation() {
	
	FunctionInitialCond<float> *zero = new FunctionInitialCond<float>([] (float,float,float) -> float { return 0;});
	CircleInitialCond<float> *circle = new CircleInitialCond<float>(0.1,0.75,0.75,0.5);

	std::map<std::string, InitialCond<float>*> initialConds;
	initialConds.emplace("r", zero);
	initialConds.emplace("e", circle);

	initOpenClContext();
	initGrids(initialConds);
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
		
	for(cl::Device &dev : _devices) {
		DeviceThread<> devThread(this, _platform, _context, dev);
		_deviceThreads.push_back(devThread);
	}

	log_console->infoStream() << "Created " << _nDevices << " device threads !";
}
void MultiGpu::createGlObjects(){}
		

void MultiGpu::initGrids(const std::map<std::string, InitialCond<float>*> &initialConditions) {
		
		_gridWidth = 511u;
		_gridHeight = 123u;
		_gridLength = 769;
		_nFunctions = initialConditions.size();

		for(auto &initialConds : initialConditions) {
			MultiBufferedDomain<float,2u> dom(_gridWidth, _gridHeight, _gridLength, 1u, 4, initialConds.second);
			_domains.emplace(initialConds.first, dom);
		}
}

