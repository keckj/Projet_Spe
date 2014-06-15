
#include "log.hpp"
#include "glUtils.hpp"

using namespace log4cpp;

namespace utils {

	bool contextError = false;

	void glAssert(const std::string &file, int line, bool abort) {

		GLenum error = glGetError();

		switch(error) {
			case GL_NO_ERROR:
				break; 
			case GL_INVALID_ENUM:
				log_console->errorStream() << "OpenGL error : " << file << ":" << line << ":" << "GL_INVALID_ENUM\n\t\t"
					"An unacceptable value is specified for an enumerated argument. "
					"The offending command is ignored and has no other side effect than to set the error flag.";
				break;

			case GL_INVALID_VALUE:
				log_console->errorStream() << "OpenGL error : " << file << ":" << line << ":" << "GL_INVALID_VALUE\n\t\t"
					"A numeric argument is out of range."
					"The offending command is ignored and has no other side effect than to set the error flag.";
				break;
			case GL_INVALID_OPERATION:
				log_console->errorStream() << "OpenGL error : " << file << ":" << line << ":" << "GL_INVALID_OPERATION\n\t\t"
					"The specified operation is not allowed in the current state."
					"The offending command is ignored and has no other side effect than to set the error flag.";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				log_console->errorStream() << "OpenGL error : " << file << ":" << line << ":" << "GL_INVALID_FRAMEBUFFER_OPERATION\n\t\t"
					"The framebuffer object is not complete. "
					"The offending command is ignored and has no other side effect than to set the error flag.";
				break;
			case GL_OUT_OF_MEMORY:
				log_console->errorStream() << "OpenGL error : " << file << ":" << line << ":" << "GL_OUT_OF_MEMORY\n\t\t"
					"There is not enough memory left to execute the command. The state of the case GL"
					"The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
					break;
			case GL_STACK_UNDERFLOW:
				log_console->errorStream() << "OpenGL error : " << file << ":" << line << ":" << "GL_STACK_UNDERFLOW\n\t\t"
					"An attempt has been made to perform an operation that would cause an internal stack to underflow.";
				break;
			case GL_STACK_OVERFLOW:
				log_console->errorStream() << "OpenGL error : " << file << ":" << line << ":" << "GL_STACK_OVERFLOW\n\t\t"
					"An attempt has been made to perform an operation that would cause an internal stack to overflow.";
				break;
			default:
				log_console->errorStream() << "OpenGL error : " << file << ":" << line << "\n\t\t"
					"Unknown error !";
		}

		if(error != GL_NO_ERROR && abort)
			exit(EXIT_FAILURE);
	}

	bool isExtensionSupported(const char *extList, const char *extension)
	{
		const char *start;
		const char *where, *terminator;

		where = strchr(extension, ' ');
		if (where || *extension == '\0')
			return false;

		for (start=extList;;) {
			where = strstr(start, extension);

			if (!where)
				break;

			terminator = where + strlen(extension);

			if ( where == start || *(where - 1) == ' ' )
				if ( *terminator == ' ' || *terminator == '\0' )
					return true;

			start = terminator;
		}

		return false;
	}

	typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
	void createOpenGLContext(Display **display, GLXContext *ctx, Window *win, Colormap *cmap, GLXContext shareList) {

		if(shareList == 0)
			log_console->infoStream()  << "Creating an openGL context !";
		else
			log_console->infoStream()  << "Creating a shared openGL/openGL context !";

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

		//log_console->infoStream() <<  "Mapping window...";
		//XMapWindow((*display), (*win));

		const char *glxExts = glXQueryExtensionsString((*display), DefaultScreen((*display)));
		glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
		glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) 
			glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

		//change error handler to handle fail
		contextError = false;
		int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&contextErrorHandler);

		(*ctx) = 0;
		if (!utils::isExtensionSupported(glxExts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB)
		{
			log_console->infoStream() 
				<<  "glXCreateContextAttribsARB() not found... "
				<< "Using old-style GLX context";
			(*ctx) = glXCreateNewContext( (*display), bestFbc, GLX_RGBA_TYPE, shareList, True );
		}
		else
		{
			int context_attribs[] =
			{
				GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
				GLX_CONTEXT_MINOR_VERSION_ARB, 1,
				None
			};

			log_console->infoStream() <<  "Creating context...";
			(*ctx) = glXCreateContextAttribsARB((*display), bestFbc, shareList,
					True, context_attribs);

			//sync errors
			XSync((*display), False);
			if (!contextError && (*ctx))
				log_console->infoStream() <<  "Created GL 3.1 context !";
			else
			{
				context_attribs[1] = 1;
				context_attribs[3] = 0;
				contextError = false;

				log_console->infoStream() <<  "Failed to create GL 3.0 context"
					<< 	" ... using old-style GLX context !" ;
				(*ctx) = glXCreateContextAttribsARB( (*display), bestFbc, shareList, 
						True, context_attribs );
			}
		}

		//sync errors
		XSync( (*display), False );

		//restore error handler
		XSetErrorHandler(oldHandler);

		if (contextError || !(*ctx)){
			log_console->infoStream() <<  "Failed to create an OpenGL context !";
			exit(EXIT_FAILURE);
		}

		// Verifying that context is a direct context
		if ( ! glXIsDirect ( (*display), (*ctx) ) )
			log_console->infoStream() <<  "Indirect GLX rendering context obtained !";
		else
			log_console->infoStream() <<  "Direct GLX rendering context obtained !";

		//EXAMPLE :
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

	int contextErrorHandler(Display *dpy, XErrorEvent *ev)
	{
		log_console->warnStream() << "OpenGL context error " << ev->error_code << " !";
		contextError = true;
		return 0;
	}

}
