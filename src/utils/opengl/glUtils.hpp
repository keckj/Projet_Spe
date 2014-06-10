
#ifndef GLUTILS_H
#define GLUTILS_H

#include "headers.hpp"

namespace utils {
		void glAssert(const std::string &file, int line, bool abort = true);
		void createOpenGLContext(Display **display, GLXContext *ctx, Window *win, Colormap *cmap, GLXContext shareList);
		bool isExtensionSupported(const char *extList, const char *extension);

		int contextErrorHandler(Display *dpy, XErrorEvent *ev);
}

#endif /* end of include guard: GLUTILS_H */
