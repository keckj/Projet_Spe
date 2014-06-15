
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
		std::map<QString, bool> *renderedVars,
		std::map<QString, int> *initialCondsId) :
	Model(nbIter, renderedVars, initialCondsId, width_, height_, length_),
	_step(false),
	_stepDone(false),
	_nDevices(0),
	_nFunctions(0),
	_init(false),
	_sliceIdX(0), _sliceIdY(0), _sliceIdZ(0),
	_args(args),
	_kill(false),
	_destroyed(false)
{
}

MultiGpu::~MultiGpu() 
{
	killWorkerThreads();
	assert(glXMakeCurrent(Model::solverDisplay, Model::solverWindow, Model::solverContext));
	for(auto k : m_mappedTextures.keys())
		glDeleteTextures(1, &m_mappedTextures[k]);
	glXMakeCurrent(Model::solverDisplay, 0, 0);
}

void MultiGpu::initComputation() {
	initGrids();
	initOpenClContext();
	waitInit();
}

void MultiGpu::computeStep(int i) {
	giveStep();
	waitStep();
}

void MultiGpu::finishComputation() {
}

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
		abort();
	}
	if(_nDevices == 0) {
		log_console->infoStream() << "A platform has openGL/openCL sharing capabilities, but no supported devices found !";
		abort();
	}


	//get solver openGL context created by qt
	log_console->infoStream() << "Making solver openGL context current !";

	assert(glXMakeCurrent(Model::solverDisplay, Model::solverWindow, Model::solverContext));
	{
		//Create context for supported devices
		cl_context_properties contextProperties[] = { 
			CL_CONTEXT_PLATFORM, (cl_context_properties) _platform(), 
			CL_GL_CONTEXT_KHR, (cl_context_properties) Model::solverContext,
			CL_GLX_DISPLAY_KHR, (cl_context_properties) Model::solverDisplay, 
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
	glXMakeCurrent(Model::solverDisplay, 0, 0);

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
		DeviceThread<N_COMMANDQUEUES> *dt = new DeviceThread<N_COMMANDQUEUES>(this, _platform, _context, program, dev, fence, _args, 1.0f/this->m_width, true);
		std::thread t(*dt);
		_deviceThreads.push_back(dt);
		t.detach();
		i++;
	}

	log_console->infoStream() << "Created " << devicesNeeded << " device threads, " << _devices.size()-devicesNeeded <<" devices will be idle !";
}

void MultiGpu::initGrids() {

	unsigned int gridWidth = this->m_width;
	unsigned int gridHeight = this->m_height;
	unsigned int gridLength = this->m_length;
	_nFunctions = this->m_initialConds.size();

	for(auto initialConds : this->m_initialConds) {
		MultiBufferedDomain<float,1u> *dom = 
			new MultiBufferedDomain<float,1u>(gridWidth, gridHeight, gridLength, 1u, 4, initialConds.second);
		_domains.emplace(std::string((char*)initialConds.first.data()), dom);
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
		std::unique_lock<std::mutex> lock(_domainMutex);
		_domainCond.notify_one();
		if (_availableDomains.size() == 0) {
			return false;
		}

		id = _availableDomains.front();
		_availableDomains.pop_front();
	}

	for(std::pair<std::string,MultiBufferedDomain<float,1u>*> subdom : _domains) {
		subDomain.emplace(subdom.first, (*subdom.second)[id]);
	}

	return true;
}

void MultiGpu::releaseSubDomain(std::map<std::string, MultiBufferedSubDomain<float,1u>*> subDomain){
	std::unique_lock<std::mutex> lock(_domainMutex);
	assert(_availableDomains.size()<=_splits);
	_availableDomains.push_back(subDomain.begin()->second->id());
	_domainCond.notify_one();
}

void MultiGpu::initDone() {
	std::unique_lock<std::mutex> lock(_mutex);
	_init = true;
	_cond.notify_all();
}


void MultiGpu::stepDone() {
	renderToTextures();
}



void MultiGpu::renderToTextures() {
	assert(glXMakeCurrent(Model::solverDisplay, Model::solverWindow, Model::solverContext));

	this->m_mappedTexturesGui.clear();

	glActiveTexture(GL_TEXTURE0);
	for (auto pair : _domains) {
		QString name(pair.first.c_str());
		if(!(*this->m_renderedVars)[name])
				continue;
		
		float *data = _sliceZ[pair.first];

		unsigned int texture = this->m_mappedTextures[name];
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 
				this->m_width, this->m_height, 0, 
				GL_LUMINANCE, GL_FLOAT, (GLvoid*) data);
		assert(glIsTexture(texture));

		this->m_mappedTexturesGui.insert(name, texture);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	CHK_GL_ERRORS();

	glXMakeCurrent(Model::solverDisplay, 0, 0);
}
		
	//DEBUG
	//static unsigned int k = 0;
	//k++;
	
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
        
std::map<QString, int> *MultiGpu::getDefaultInit() {
	std::map<QString, int> *conds = new std::map<QString, int>;
	conds->emplace("e", 6);
	conds->emplace("r", 7);

	return conds;
}

void MultiGpu::abort() {
	emit finished();
}

void MultiGpu::giveStep() {
	std::unique_lock<std::mutex> lock(_mutex);
	_step = true;
	_stepDone = false;
	_cond.notify_all();
}
void MultiGpu::lockStep() {
	std::unique_lock<std::mutex> lock(_mutex);
	_step = false;
	_cond.notify_all();
}
void MultiGpu::releaseStep() {
	std::unique_lock<std::mutex> lock(_mutex);
	_stepDone = true;
	_cond.notify_all();
}
void MultiGpu::destroy() {
	std::unique_lock<std::mutex> lock(_mutex);
	_destroyed = true;
	_cond.notify_all();
}


void MultiGpu::waitStep() {
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]{return _stepDone||_kill;});
	_cond.notify_all();
}

void MultiGpu::waitCompute() {
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]{return _step||_kill;});
	_cond.notify_all();
}
		
void MultiGpu::waitInit() {
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]{return _init||_kill;});
	_cond.notify_all();
}

void MultiGpu::killWorkerThreads() {
	_kill = true;
	for(auto t : _deviceThreads)
		t->finish();
	_cond.notify_all();
	
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]{return _destroyed;});
	_cond.notify_all();
}
		
