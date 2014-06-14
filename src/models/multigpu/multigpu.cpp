
#include "headers.hpp"
#include "multigpu.hpp"
#include "multigpu.moc"
#include "log.hpp"
#include "domain.hpp"
#include "circleInitialCond.hpp"
#include "functionInitialCond.hpp"
#include "openGLScene.hpp"

#include <sstream>
#include <algorithm>

using namespace log4cpp;
using namespace utils;
		
MultiGpu::MultiGpu(int nbIter, 
		unsigned int width_, unsigned int height_, unsigned int length_,
		std::map<QString, Argument> *args, 
		std::map<QString, bool> *renderedVars) : 

	Model(nbIter, renderedVars, width_, height_, length_),
	_step(false),
	_stepDone(false),
	_nDevices(0),
	_nFunctions(0),
	_init(false),
	_sliceIdX(0), _sliceIdY(0), _sliceIdZ(0),
	_args(args)
{
}

MultiGpu::~MultiGpu() 
{
	glXMakeCurrent(OpenGLScene::solverDisplay, 0, 0);
	glXDestroyContext(OpenGLScene::solverDisplay, OpenGLScene::solverContext);
	XDestroyWindow(OpenGLScene::solverDisplay, OpenGLScene::solverWindow);
	XFreeColormap(OpenGLScene::solverDisplay, OpenGLScene::solverColormap);
	XCloseDisplay(OpenGLScene::solverDisplay);
}

void MultiGpu::initComputation() {
	
	FunctionInitialCond<float> *zero = new FunctionInitialCond<float>([] (float,float,float)->float {return 0;});
	FunctionInitialCond<float> *one = new FunctionInitialCond<float>([] (float,float,float)->float {return 1;});
	FunctionInitialCond<float> *half = new FunctionInitialCond<float>([] (float,float,float)->float {return 1/2.0;});
	CircleInitialCond<float> *circle = new CircleInitialCond<float>(0.1,0.5,0.5,0.5);
	FunctionInitialCond<float> *sine = new FunctionInitialCond<float>([] (float x,float y,float)->float {return abs(cos(2*3.14*2*x)*cos(2*3.14*2*y));});
	FunctionInitialCond<float> *halfPlane = new FunctionInitialCond<float>([] (float x,float y,float)->float 
			{return (x<=0.5 && y<=0.5)||(x>=0.5 && y>=0.5);});
	FunctionInitialCond<float> *halfPlaneX = new FunctionInitialCond<float>([] (float x,float y,float)->float 
			{return (x<=0.5);});
	FunctionInitialCond<float> *halfPlaneY = new FunctionInitialCond<float>([] (float x,float y,float)->float 
			{return (y<=0.5);});
	FunctionInitialCond<float> *gradient = new FunctionInitialCond<float>([](float x,float y, float)->float
			{return x*y;});
	FunctionInitialCond<float> *gradientX = new FunctionInitialCond<float>([](float x,float y, float)->float
			{return x;});
	FunctionInitialCond<float> *gradientY = new FunctionInitialCond<float>([](float x,float y, float)->float
			{return y;});

	std::map<std::string, InitialCond<float>*> initialConds;
	initialConds.emplace("e", gradientX);
	initialConds.emplace("r", gradientY);

	initGrids(initialConds);

	initOpenClContext();
}

void MultiGpu::computeStep(int i) {
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_step = true;
		_stepDone = false;
		_cond.wait(lock, [this]{return _stepDone;});
		_cond.notify_one();
	}
}

void MultiGpu::finishComputation() {;}

void MultiGpu::initOpenClContext(){
	
	cl_int err;
	
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


	//get solver openGL context created by qt
	log_console->infoStream() << "Making solver openGL context current !";

	assert(glXMakeCurrent(OpenGLScene::solverDisplay, OpenGLScene::solverWindow, OpenGLScene::solverContext));
	{
		//print infos about current openGL context
		int glxConfigXid=0, glxSupportedRenderType=0, glxScreenNumber=0;
		glXQueryContext(OpenGLScene::solverDisplay, OpenGLScene::solverContext,  GLX_FBCONFIG_ID, &glxConfigXid);
		glXQueryContext(OpenGLScene::solverDisplay, OpenGLScene::solverContext, GLX_RENDER_TYPE, &glxSupportedRenderType);
		glXQueryContext(OpenGLScene::solverDisplay, OpenGLScene::solverContext, GLX_SCREEN, &glxScreenNumber);

		log_console->infoStream() << "Solver openGL context : "
			<< " XID=" << glxConfigXid
			<< " RenderType=" << glxSupportedRenderType
			<< " ScreenNumber=" << glxScreenNumber;

		//Create context for supported devices
		cl_context_properties contextProperties[] = { 
			CL_CONTEXT_PLATFORM, (cl_context_properties) _platform(), 
			CL_GL_CONTEXT_KHR, (cl_context_properties) OpenGLScene::solverContext,
			CL_GLX_DISPLAY_KHR, (cl_context_properties) OpenGLScene::solverDisplay, 
			0 
		};

		struct ContextUserData *callbackData = new struct ContextUserData;
		*callbackData = {&_platform, CL_DEVICE_TYPE_GPU,0};

		_context = cl::Context(_devices, contextProperties, openclContextCallback, (void*)callbackData, &err);
		if(err == CL_INVALID_OPERATION) {
			log_console->warnStream() << "Impossible to create a shared openCL/openGL context, switching to a normal openCL context.";
			contextProperties[2] = (cl_context_properties) 0; 
			_context = cl::Context(_devices, contextProperties, openclContextCallback, (void*)callbackData, &err);
		}
		CHK_ERRORS(err);

		createTextures();
		CHK_GL_ERRORS();
	}
	glXMakeCurrent(OpenGLScene::solverDisplay, 0, 0);

	//Alloc host side texture data
	allocSlices();

	//Load sources
	cl::Program::Sources sources = utils::loadSourcesFromFile("src/kernels/multigpu.cl");	

	//Make program
	cl::Program program(_context, sources, &err); CHK_ERRORS(err);
	utils::buildProgram(program, _devices, "", "MultiGpu Kernel");

	unsigned int devicesNeeded = std::min<unsigned int>(_domains.begin()->second->nSplits(), _devices.size());
	Fence *fence = new Fence(devicesNeeded);

	unsigned int i = 0;
	for(auto dev : _devices) {
		if(i >= devicesNeeded)
			break;
		DeviceThread<1u> *dt = new DeviceThread<1u>(this, _platform, _context, program, dev, fence, _args, 1.0f/this->m_width, true);
		std::thread t(*dt);
		t.detach();
		i++;
	}
	
	log_console->infoStream() << "Created " << devicesNeeded << " device threads, " << _devices.size()-devicesNeeded <<" devices will be idle !";
}
		
void MultiGpu::initGrids(const std::map<std::string, InitialCond<float>*> &initialConditions) {
		
		unsigned int gridWidth = this->m_width;
		unsigned int gridHeight = this->m_height;
		unsigned int gridLength = this->m_length;
		_nFunctions = initialConditions.size();
	
		for(auto &initialConds : initialConditions) {
			MultiBufferedDomain<float,1u> *dom = 
				new MultiBufferedDomain<float,1u>(gridWidth, gridHeight, gridLength, 1u, 64, initialConds.second);
			_domains.emplace(initialConds.first, dom);
		}
			
		_splits = _domains.begin()->second->nSplits();		
		resetSubDomains();
}

void MultiGpu::resetSubDomains() {
		for (unsigned int i = 0; i < _splits; i++) {
			_availableDomains.push_back(i);
		}
}
		
		
bool MultiGpu::subDomainAvailable() {
		return _availableDomains.size() > 0;
}
		
bool MultiGpu::tryToTakeSubDomain(std::map<std::string, MultiBufferedSubDomain<float,1u>*> &subDomain) {
	
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
	
	for(std::pair<std::string,MultiBufferedDomain<float,1u>*> subdom : _domains) {
		subDomain.emplace(subdom.first, (*subdom.second)[id]);
	}

	return true;
}

void MultiGpu::releaseSubDomain(std::map<std::string, MultiBufferedSubDomain<float,1u>*> subDomain){
	std::unique_lock<std::mutex> lock(_mutex);
	assert(_availableDomains.size()<=_splits);
	_availableDomains.push_back(subDomain.begin()->second->id());
	_cond.notify_one();
}

void MultiGpu::initDone() {
	resetSubDomains();
	_init = true;
	log_console->infoStream() << "Init done";
}

void MultiGpu::stepDone() {
	{
		std::unique_lock<std::mutex> lock(_mutex);
		renderToTextures();
		_step = false;
		_stepDone = true;
		_cond.notify_one();
	}
}



void MultiGpu::renderToTextures() {
	assert(glXMakeCurrent(OpenGLScene::solverDisplay, OpenGLScene::solverWindow, OpenGLScene::solverContext));

	//DEBUG
	//static unsigned int k = 0;
	//k++;
	
	glActiveTexture(GL_TEXTURE0);
	for (auto pair : _domains) {
		float *data = _sliceZ[pair.first];

		//DEBUG
		//for (unsigned int j = 0; j < this->m_height ; j++) {
			//for (unsigned int i = 0; i < this->m_width; i++) {
				//float ii = float(i)/this->m_width;
				//float jj = float(j)/this->m_height;

				//data[j*this->m_width+i] = (k%2==0 ? 
						//((ii<1/2.0f && jj <1/2.0f) || (ii>1/2.0f && jj>1/2.0f)) :
						//!((ii<1/2.0f && jj <1/2.0f) || (ii>1/2.0f && jj>1/2.0f))
						//);
			//}
		//}
		//k++;
		
		unsigned int texture = this->m_mappedTextures[QString(pair.first.c_str())];
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 
				this->m_width, this->m_height, 0, 
				GL_LUMINANCE, GL_FLOAT, (GLvoid*) data);
		assert(glIsTexture(texture));
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	CHK_GL_ERRORS();
		
	glXMakeCurrent(OpenGLScene::solverDisplay, 0, 0);
}
		
unsigned int MultiGpu::sliceIdX() {
			return _sliceIdX;
}
unsigned int MultiGpu::sliceIdY() {
			return _sliceIdY;
}
unsigned int MultiGpu::sliceIdZ() {
			return _sliceIdZ;
}
std::map<std::string, float*> MultiGpu::sliceX() {
			return _sliceX;
}
std::map<std::string, float*> MultiGpu::sliceY() {
			return _sliceY;
}
std::map<std::string, float*> MultiGpu::sliceZ() {
			return _sliceZ;
}
	
void MultiGpu::allocSlices() {
	for(auto pair : _domains) {
		_sliceX.emplace(pair.first, pair.second->allocateSliceX());	
		_sliceY.emplace(pair.first, pair.second->allocateSliceY());	
		_sliceZ.emplace(pair.first, pair.second->allocateSliceZ());	
	}
}
	
void MultiGpu::createTextures() {
	
		unsigned int *textures = new unsigned int[_nFunctions];
	
		log_console->infoStream() << "Creating " << _nFunctions << " model textures...";
		glGenTextures(_nFunctions, textures);
		glFinish();
		CHK_GL_ERRORS();

		unsigned int i = 0;
		for (auto pair : _domains) {
			log_console->infoStream() << "Created texture " << textures[i];
			this->m_mappedTextures.insert(QString(pair.first.c_str()), textures[i++]);
		}
}

std::map<QString, Argument> *MultiGpu::getArguments() {
	
	std::map<QString,Argument> *args = new std::map<QString,Argument>;

	args->emplace("epsilon", Argument(0.01f, 0.001f, 0.1f, WidgetType::SPINBOX));
	args->emplace("d",Argument(5e-5f, 1e-5f, 1e-4f, WidgetType::SPINBOX));
	args->emplace("k",Argument(8.0f, 0.0f, 20.0f, WidgetType::SPINBOX));
	args->emplace("mu_1",Argument(0.07f, 0.01f, 0.5f, WidgetType::SPINBOX));
	args->emplace("mu_2",Argument(0.3f, 0.1f, 0.5f, WidgetType::SPINBOX));
	args->emplace("alpha_1",Argument(0.2f, 0.1f, 0.5f, WidgetType::SPINBOX));
	args->emplace("alpha_2",Argument(0.1f, 0.1f, 0.5f, WidgetType::SPINBOX));
			
	return args;
}
       
std::map<QString, bool> *MultiGpu::getVariables() {
    std::map<QString, bool> *vars = new std::map<QString, bool>;
    vars->emplace("e", true);
    vars->emplace("r", false);

    return vars;
}
		
void MultiGpu::abort() {
	emit finished();
}
