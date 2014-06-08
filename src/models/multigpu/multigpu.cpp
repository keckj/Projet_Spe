
#include "headers.hpp"
#include "multigpu.hpp"
#include "multigpu.moc"
#include "log.hpp"
#include "domain.hpp"
#include "circleInitialCond.hpp"
#include "functionInitialCond.hpp"
#include "openGLScene.hpp"

#include <sstream>

using namespace log4cpp;
using namespace utils;
		
bool MultiGpu::_contextError = false;

MultiGpu::MultiGpu(int nbIter) : 
	Model(10),
	_nDevices(0),
	_nFunctions(0),
	_gridWidth(0), _gridHeight(0), _gridLength(0),
	_init(false),
	_grid(0),
	_display(0)
{
}

MultiGpu::~MultiGpu() 
{
	std::unique_lock<std::mutex> lock(OpenGLScene::glMutex);
	glXMakeCurrent(_display, 0, 0);
	glXDestroyContext(_display, _ctx);
	XDestroyWindow(_display, _win);
	XFreeColormap(_display, _cmap);
	XCloseDisplay(_display);
	OpenGLScene::glConditionVariable.notify_one();
}

void MultiGpu::initComputation() {
	
	FunctionInitialCond<float> *zero = new FunctionInitialCond<float>([] (float,float,float)->float {return 0;});
	FunctionInitialCond<float> *one = new FunctionInitialCond<float>([] (float,float,float)->float {return 1;});
	CircleInitialCond<float> *circle = new CircleInitialCond<float>(0.1,0.5,0.5,0.5);
	FunctionInitialCond<float> *sine = new FunctionInitialCond<float>([] (float x,float y,float)->float {return abs(cos(2*3.14*2*x)*cos(2*3.14*2*y));});
	FunctionInitialCond<float> *halfPlane = new FunctionInitialCond<float>([] (float x,float y,float)->float 
			{return (x<=0.5 && y<=0.5)||(x>0.5 && y>0.5);});

	std::map<std::string, InitialCond<float>*> initialConds;
	initialConds.emplace("e", circle);
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


	//Create openGL context, try to get shared context with openCL
	{
		std::unique_lock<std::mutex> lock(OpenGLScene::glMutex);
		
		createOpenGLContext(&_display, &_ctx, &_win, &_cmap);
		glXMakeCurrent(_display, _win, _ctx);

		//print infos about current openGL context
		int glxConfigXid=0, glxSupportedRenderType=0, glxScreenNumber=0;
		glXQueryContext(_display, _ctx,  GLX_FBCONFIG_ID, &glxConfigXid);
		glXQueryContext(_display, _ctx, GLX_RENDER_TYPE, &glxSupportedRenderType);
		glXQueryContext(_display, _ctx, GLX_SCREEN, &glxScreenNumber);

		log_console->infoStream() << "Current openGL context : "
			<< " XID=" << glxConfigXid
			<< " RenderType=" << glxSupportedRenderType
			<< " ScreenNumber=" << glxScreenNumber;

		//Create context for supported devices
		cl_context_properties contextProperties[] = { 
			CL_CONTEXT_PLATFORM, (cl_context_properties) _platform(), 
			CL_GL_CONTEXT_KHR, (cl_context_properties) _ctx,
			CL_GLX_DISPLAY_KHR, (cl_context_properties) _display, 
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

		createTextures();
	
		OpenGLScene::glConditionVariable.notify_one();
	}
	allocSlices();

	//Load sources
	cl::Program::Sources sources = utils::loadSourcesFromFile("src/kernels/multigpu.cl");	

	//Make program
	cl_int err;
	cl::Program program(_context, sources, &err); CHK_ERRORS(err);
	utils::buildProgram(program, _devices, "", "MultiGpu Kernel");

	log_console->infoStream() << "Created " << _nDevices << " device threads !";
	
	Fence *fence = new Fence(_devices.size());
	for(auto dev : _devices) {
		DeviceThread<1u> *dt = new DeviceThread<1u>(this, _platform, _context, program, dev, fence);
		std::thread t(*dt);
		t.detach();
	}
}
		
void MultiGpu::initGrids(const std::map<std::string, InitialCond<float>*> &initialConditions) {
		
		_gridWidth = 256u;
		_gridHeight = 256u;
		_gridLength = 1u;
		_nFunctions = initialConditions.size();
	
		for(auto &initialConds : initialConditions) {
			MultiBufferedDomain<float,1u> *dom = 
				new MultiBufferedDomain<float,1u>(_gridWidth, _gridHeight, _gridLength, 1u, 4, initialConds.second);
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
	std::cout << "INIT DONE !!" << std::endl;

	_grid = _domains.begin()->second->toGrid(0); 
    stepComputed(dynamic_cast<Grid2D<float>*>(_grid));

	_init = true;
	resetSubDomains();
}
		
void MultiGpu::stepDone() {
	delete _grid;
	_grid = _domains.begin()->second->toGrid(0);
	renderToTextures();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stepComputed(dynamic_cast<Grid2D<float>*>(_grid));
}
		

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
void MultiGpu::createOpenGLContext(Display **display, GLXContext *ctx, Window *win, Colormap *cmap) {
  
	log_console->infoStream()  << "Creating an openGL context !";
	
	static int visual_attribs[] =
	{
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		None
	};

	(*display) = XOpenDisplay(NULL);
	if (!(*display)) {
		log_console->errorStream()  << "Failed to open X (*display), aborting !";
		exit(EXIT_FAILURE);
	}
  
	int glx_major, glx_minor;
	if (!glXQueryVersion((*display), &glx_major, &glx_minor) 
			|| ((glx_major == 1) && (glx_minor< 3)) 
			|| ( glx_major < 1 ) ) {
		log_console->errorStream() << "Invalid GLX version : " << glx_major << "." << glx_minor << " (1.3 min)";
		exit(EXIT_FAILURE);
	}
  
	int fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig((*display), DefaultScreen((*display)), visual_attribs, &fbcount);
	if (!fbc) {
		log_console->errorStream() << "Failed to retrieve a framebuffer config !";
		exit(EXIT_FAILURE);
	}
	log_console->infoStream() <<  "Found " << fbcount << " matching framebuffers configs !";
  
	log_console->infoStream() <<  "Getting XVisualInfos...";
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
	int i;
	for (i=0; i<fbcount; ++i)
	{
		XVisualInfo *vi = glXGetVisualFromFBConfig((*display), fbc[i]);
		if (vi) {
			int samp_buf, samples;
			glXGetFBConfigAttrib((*display), fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
			glXGetFBConfigAttrib((*display), fbc[i], GLX_SAMPLES       , &samples );

			log_console->infoStream() <<  "\t\tMatching fbconfig " << i << " visual ID 0x" << vi->visualid << ": "
				<< "SAMPLE_BUFFERS = " << samp_buf << " SAMPLES = " << samples;

			if (best_fbc < 0 || (samp_buf && samples > best_num_samp))
				best_fbc = i, best_num_samp = samples;
			if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
				worst_fbc = i, worst_num_samp = samples;
		}
		XFree(vi);
	}

	GLXFBConfig bestFbc = fbc[best_fbc];
	XFree(fbc);
  
	// Get a visual
	XVisualInfo *vi = glXGetVisualFromFBConfig( (*display), bestFbc );
	log_console->infoStream() <<  "Chosen visual ID = 0x" <<  vi->visualid;
	log_console->infoStream() <<  "Creating colormap...";
	XSetWindowAttributes swa;
	swa.colormap = (*cmap) = XCreateColormap( (*display),
			RootWindow( (*display), vi->screen ), 
			vi->visual, AllocNone );
	swa.background_pixmap = None ;
	swa.border_pixel      = 0;
	swa.event_mask        = StructureNotifyMask;
  
	log_console->infoStream() <<  "Creating window...";
	(*win) = XCreateWindow( (*display), RootWindow( (*display), vi->screen ), 
			0, 0, 100, 100, 0, vi->depth, InputOutput, 
			vi->visual, 
			CWBorderPixel|CWColormap|CWEventMask, &swa );
	if (!(*win)) {
		log_console->infoStream() <<  "Failed to create (*win)dow !";
		exit(EXIT_FAILURE);
	}
  
	XFree(vi);
	XStoreName((*display), (*win), "GL 3.0 window");

	log_console->infoStream() <<  "Mapping window...";
	XMapWindow((*display), (*win));
  
	const char *glxExts = glXQueryExtensionsString((*display), DefaultScreen((*display)));
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) 
		glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

	//change error handler to handle fail
	_contextError = false;
	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&contextErrorHandler);

	(*ctx) = 0;
	if (!utils::isExtensionSupported(glxExts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB)
	{
		log_console->infoStream() 
			<<  "glXCreateContextAttribsARB() not found... "
			<< "Using old-style GLX context";
		(*ctx) = glXCreateNewContext( (*display), bestFbc, GLX_RGBA_TYPE, 0, True );
	}
	else
	{
		int context_attribs[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 3,
			None
		};

		log_console->infoStream() <<  "Creating context...";
		(*ctx) = glXCreateContextAttribsARB( (*display), bestFbc, 0,
				True, context_attribs );

		//sync errors
		XSync( (*display), False );
		if ( !_contextError && (*ctx) )
			log_console->infoStream() <<  "Created GL 3.3 context !";
		else
		{
			context_attribs[1] = 1;
			context_attribs[3] = 0;
			_contextError = false;

			log_console->infoStream() <<  "Failed to create GL 3.0 context"
				<< 	" ... using old-style GLX context !" ;
			(*ctx) = glXCreateContextAttribsARB( (*display), bestFbc, 0, 
					True, context_attribs );
		}
	}
  
	//sync errors
	XSync( (*display), False );

	//restore error handler
	XSetErrorHandler(oldHandler);
  
	if (_contextError || !(*ctx)){
		log_console->infoStream() <<  "Failed to create an OpenGL context !";
		exit(EXIT_FAILURE);
	}
  
	// Verifying that context is a direct context
	if ( ! glXIsDirect ( (*display), (*ctx) ) )
	{
		log_console->infoStream() <<  "Indirect GLX rendering context obtained !";
	}
	else
	{
		log_console->infoStream() <<  "Direct GLX rendering context obtained !";
	}
  
	//glXMakeCurrent( (*display), (*win), (*ctx) );

	//glClearColor( 0, 0.5, 1, 1 );
	//glClear( GL_COLOR_BUFFER_BIT );
	//glXSwapBuffers ( (*display), (*win) );
	
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//glClearColor ( 1, 0.5, 0, 1 );
	//glClear ( GL_COLOR_BUFFER_BIT );
	//glXSwapBuffers ( (*display), (*win) );
		
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

int MultiGpu::contextErrorHandler(Display *dpy, XErrorEvent *ev)
{
	log_console->warnStream() << "OpenGL context error " << ev->error_code << " !";
	_contextError = true;
	return 0;
}

void MultiGpu::createTextures() {
	unsigned int *textures = new unsigned int[_nFunctions];
	glGenTextures(_nFunctions, textures);

	unsigned int i = 0;
	for (auto pair : _domains) {
		_mapped_textures.emplace(pair.first, textures[i++]);
	}
}

void MultiGpu::renderToTextures() {
	std::unique_lock<std::mutex> lock(OpenGLScene::glMutex);

	//get openGL context
	glXMakeCurrent(_display, _win, _ctx);
	
	OpenGLScene::glConditionVariable.notify_one();
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
