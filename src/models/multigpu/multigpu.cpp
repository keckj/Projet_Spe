
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
	_gridWidth(0), _gridHeight(0), _gridLength(0),
	_init(false)
{
}

MultiGpu::~MultiGpu() 
{
}

void MultiGpu::initComputation() {
	
	FunctionInitialCond<float> *zero = new FunctionInitialCond<float>([] (float,float,float)->float {return 0;});
	CircleInitialCond<float> *circle = new CircleInitialCond<float>(0.7,0.5,0.5,0.5);
	FunctionInitialCond<float> *one = new FunctionInitialCond<float>([] (float,float,float)->float {return 1;});
	FunctionInitialCond<float> *sine = new FunctionInitialCond<float>([] (float x,float y,float)->float {return abs(cos(2*3.14*2*x)*cos(2*3.14*2*y));});
	FunctionInitialCond<float> *halfPlane = new FunctionInitialCond<float>([] (float x,float,float)->float {return x<=0.5;});

	std::map<std::string, InitialCond<float>*> initialConds;
	initialConds.emplace("e", sine);
	initialConds.emplace("r", zero);

	initGrids(initialConds);

	initOpenClContext();
}

void MultiGpu::computeStep(int i) {
	while(!_init)
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	log_console->infoStream() << "LOCKED IN COMPUTE STEP !";

	while(true)
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

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

	log_console->infoStream() << "Created " << _nDevices << " device threads !";
	
	Fence *fence = new Fence(_devices.size());
	for(auto dev : _devices) {
		DeviceThread<2u> *dt = new DeviceThread<2u>(this, _platform, _context, dev, fence);
		std::thread t(*dt);
		t.detach();
	}
}
		
void MultiGpu::initGrids(const std::map<std::string, InitialCond<float>*> &initialConditions) {
		
		_gridWidth = 128u;
		_gridHeight = 128u;
		_gridLength = 1u;
		_nFunctions = initialConditions.size();
	
		for(auto &initialConds : initialConditions) {
			MultiBufferedDomain<float,2u> *dom = 
				new MultiBufferedDomain<float,2u>(_gridWidth, _gridHeight, _gridLength, 1u, 1, initialConds.second);
			_domains.emplace(initialConds.first, dom);
		}
			
		_splits = _domains.begin()->second->nSplits();		

		for (unsigned int i = 0; i < _splits; i++) {
			_availableDomains.push_back(i);
		}
}
		
bool MultiGpu::subDomainAvailable() {
		return _availableDomains.size() > 0;
}
		
bool MultiGpu::tryToTakeSubDomain(std::map<std::string, MultiBufferedSubDomain<float,2u>*> &subDomain) {
	
	subDomain.clear();
	unsigned int id;
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_cond.notify_one();
		if (!_availableDomains.size() > 0)
			return false;

		id = _availableDomains.front();
		_availableDomains.pop_front();
	}
	
	for(std::pair<std::string,MultiBufferedDomain<float,2u>*> subdom : _domains) {
		subDomain.emplace(subdom.first, (*subdom.second)[id]);
	}

	return true;
}

void MultiGpu::releaseSubDomain(std::map<std::string, MultiBufferedSubDomain<float,2u>*> subDomain){
	std::unique_lock<std::mutex> lock(_mutex);
	assert(_availableDomains.size()<=_splits);
	_availableDomains.push_back(subDomain.begin()->second->id());
	_cond.notify_one();
}

void MultiGpu::initDone() {
	std::cout << "INIT DONE !!" << std::endl;

	Grid<float>* grid = _domains.begin()->second->toGrid(0); 
    stepComputed(dynamic_cast<Grid2D<float>*>(grid));

	_init = true;
	
}
