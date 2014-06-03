
#include "log.hpp"
#include "graphicsViewer.hpp"
#include "globals.hpp"
#include "texture.hpp"
        
GraphicsViewer::GraphicsViewer() { 
	setWindowTitle(tr("GraphicsViewer")); 

	log4cpp::log_console->infoStream() << "[Glew Init] " << glewGetErrorString(glewInit());
	
	Globals::init();
	Globals::check();
	Globals::print(std::cout);

	Texture::init();
	
	log4cpp::log_console->infoStream() << "Running with OpenGL " << Globals::glVersion << " and glsl version " << Globals::glShadingLanguageVersion << " !";
	log4cpp::log_console->infoStream()<< "";
	
	//QT = row-major order
	QMatrix4x4 projectionMatrix(this->viewportTransform());
	QMatrix4x4 viewMatrix(this->transform());

	GLfloat projectionMatrixFloat[16], viewMatrixFloat[16]; 
	for (int i = 0; i < 16; i++) {
		projectionMatrixFloat[i] = GLfloat(projectionMatrix.data()[i]);
		viewMatrixFloat[i] = GLfloat(viewMatrix.data()[i]);
	}
	
	glBindBuffer(GL_UNIFORM_BUFFER, Globals::projectionViewUniformBlock);
	glBufferSubData(GL_UNIFORM_BUFFER,  0*sizeof(GLfloat), 16*sizeof(GLfloat), projectionMatrixFloat);
	glBufferSubData(GL_UNIFORM_BUFFER, 16*sizeof(GLfloat), 16*sizeof(GLfloat), viewMatrixFloat);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
