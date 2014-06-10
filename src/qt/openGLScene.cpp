#include "headers.hpp"
#include "openGLScene.hpp"
#include "openGLScene.moc" 
#include "globals.hpp"
#include "colormap.hpp"
		
Display *OpenGLScene::qtDisplay;
GLXContext OpenGLScene::qtContext;

Display *OpenGLScene::solverDisplay;
GLXContext OpenGLScene::solverContext;
Window OpenGLScene::solverWindow;
Colormap OpenGLScene::solverColormap;

using namespace log4cpp;
using namespace utils;

OpenGLScene::OpenGLScene() :
	m_drawProgram(0),
	m_drawProgramUniformLocationMap(),
	m_currentTexture(0),
	m_texCoordsVBO(0),
	m_vertexCoordsVBO(0),
	m_texture(0),
	m_colormapsUBO(),
	m_colorId(0)
{
		//Get and print info about qt context
		qtDisplay = glXGetCurrentDisplay();
		qtContext = glXGetCurrentContext();
	
		int qtGlxConfigXid=0, qtGlxSupportedRenderType=0, qtGlxScreenNumber=0;
		glXQueryContext(qtDisplay, qtContext,  GLX_FBCONFIG_ID, &qtGlxConfigXid);
		glXQueryContext(qtDisplay, qtContext, GLX_RENDER_TYPE, &qtGlxSupportedRenderType);
		glXQueryContext(qtDisplay, qtContext, GLX_SCREEN, &qtGlxScreenNumber);
	
		log_console->infoStream() << "Current QT openGL context : "
			<< " XID=" << qtGlxConfigXid
			<< " RenderType=" << qtGlxSupportedRenderType
			<< " ScreenNumber=" << qtGlxScreenNumber;

		//Create openGL context for the solver, try to get shared context with QT
		log_console->infoStream() << "Creating openGL context for the solver...";
		utils::createOpenGLContext(&solverDisplay, &solverContext, &solverWindow, &solverColormap, qtContext);
		
		makeArrays();
		makeProgram();
		makeColorMaps();
}


void OpenGLScene::updateTextures(const QMap<QString, GLuint> &texMap) {
   
    // Check if we need to change layout
	if (texMap.size() == m_texMap.size()) {
		//Notify the GUI that we have made progress
		emit stepRendered();
		return;
	}

    // Free the old map and copy the new map
    m_texMap = texMap;

    // Find best layout
    switch(texMap.size()) {
        case 0:
            m_nTexturesWidth  = 0;
            m_nTexturesHeight = 0;
            break;
        case 1:
            m_nTexturesWidth  = 1;
            m_nTexturesHeight = 1;
            break;
        case 2:
            m_nTexturesWidth  = 2;
            m_nTexturesHeight = 1;
            break;
        case 3:
        case 4:
            m_nTexturesWidth  = 2;
            m_nTexturesHeight = 2;
            break;
        case 5:
        case 6:
            m_nTexturesWidth  = 3;
            m_nTexturesHeight = 2;
            break;
        case 7:
        case 8:
        case 9:
            m_nTexturesWidth  = 3;
            m_nTexturesHeight = 3;
            break;
        default:
            log_console->warnStream() << "[OpenGLScene] Too many variables to render !";
    }

    // Buffer new arrays
    makeArrays();

    // Notify the GUI that we have made progress
    emit stepRendered();
}

void OpenGLScene::drawBackground(QPainter *painter, const QRectF &) {
	glClearColor(1.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_drawProgram->use();

	//UBOs
	glBindBufferBase(GL_UNIFORM_BUFFER, 0 , Globals::projectionViewUniformBlock);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1 , m_colormapsUBO);
	
	//VBOs
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexCoordsVBO);           
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//Uniform variables
	glUniform1i(m_drawProgramUniformLocationMap["colormapId"], m_colorId);
	glUniform1i(m_drawProgramUniformLocationMap["matrix"], 0);
	glUniform1f(m_drawProgramUniformLocationMap["minVal"], 0.0f);
	glUniform1f(m_drawProgramUniformLocationMap["maxVal"], 1.0f);
	
	//Texture unit
	glActiveTexture(GL_TEXTURE0 + 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	int i = 0;
	for (QString key :  m_texMap.keys()) {
		glBindTexture(GL_TEXTURE_2D, m_texMap[key]);
		glDrawArrays(GL_QUADS, 4*i, 4);
		++i;
	}

	glBindTexture(GL_TEXTURE_2D, 0); 
	glUseProgram(0);
	CHK_GL_ERRORS();

	renderString(0.0f,0.0f,GLUT_BITMAP_TIMES_ROMAN_24, "GOGO FAP");
	
	QTimer::singleShot(0, this, SLOT(update()));
}

void OpenGLScene::makeProgram() {
	m_drawProgram = new Program("Draw texture");
	m_drawProgram->bindAttribLocations("0 1", "vertexPos texCoord");
	m_drawProgram->bindFragDataLocation(0, "out_colour");
	m_drawProgram->bindUniformBufferLocations("0 1", "projectionView colorMaps");

	m_drawProgram->attachShader(Shader("shaders/colormap/draw_vs.glsl", GL_VERTEX_SHADER));
	m_drawProgram->attachShader(Shader("shaders/colormap/draw_fs.glsl", GL_FRAGMENT_SHADER));

	m_drawProgram->link();
	m_drawProgramUniformLocationMap = m_drawProgram->getUniformLocationsMap("colormapId minVal maxVal matrix" ,true);
}

void OpenGLScene::makeArrays() {
	//Delete unused VBOs
	if (m_texCoordsVBO)
		glDeleteBuffers(1, &m_texCoordsVBO);
	if (m_vertexCoordsVBO)
		glDeleteBuffers(1, &m_vertexCoordsVBO);
    
	//TODO dynamic gen buffers according to m_nTexturesWidth & m_nTexturesHeight 
	
	float *vertexCoords = new float[8*m_texMap.size()];
	float *textureCoords = new float[8*m_texMap.size()];
	float margin = 0.01; //TODO
	float deltaWv = 2.0 / m_nTexturesWidth; 
	float deltaHv = 2.0 / m_nTexturesHeight;
	float deltaWt = 1.0 / m_nTexturesWidth;
	float deltaHt = 1.0 / m_nTexturesHeight;

	int idx = 0;
	std::stringstream ss;
	ss << "Make arrays : ";
	for (int w = 0; w < m_nTexturesWidth; w++) {
		for (int h = 0; h < m_nTexturesHeight; h++) {
			//Do not draw a quad if there is no texture left
			if (idx == 8*m_texMap.size()) {
				break;
			}
			//1st point (top left)
			vertexCoords[idx++] = -1 + w*deltaWv    ;
			vertexCoords[idx++] =  1 - h*deltaHv    ;
			//2nd point (bottom left)
			vertexCoords[idx++] = -1 + w*deltaWv    ;
			vertexCoords[idx++] =  1 - (h+1)*deltaHv;
			//3rd point (bottom right)
			vertexCoords[idx++] = -1 + (w+1)*deltaWv;
			vertexCoords[idx++] =  1 - (h+1)*deltaHv;
			//4th point (top right)
			vertexCoords[idx++] = -1 + (w+1)*deltaWv;
			vertexCoords[idx++] =  1 - h*deltaHv    ;
	
			ss << "\n\t(w,h) = (" << w << "," << h << ")\t";
			for (int i = 0; i < 4; i++) {
				ss << " " << toStringVec3<float>(vertexCoords[idx-8+2*i], vertexCoords[idx-8+2*i+1],0);
			}
		}
	}
	log_console->infoStream() << ss.str();

	float texCoords[] = {0,0,  0,1,  1,1,  1,0};
	for (int i = 0; i < 8*m_texMap.size(); i++) {
		textureCoords[i] = texCoords[i%8];
	}

	glGenBuffers(1, &m_texCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, 8*m_texMap.size()*sizeof(float), textureCoords, GL_STATIC_DRAW);

	glGenBuffers(1, &m_vertexCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, 8*m_texMap.size()*sizeof(float), vertexCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void OpenGLScene::makeColorMaps() {
	std::map<std::string, std::pair<unsigned int, float*> > maps = ColorMap::multiHueColorMaps();

    glGenBuffers(1, &m_colormapsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_colormapsUBO);
    glBufferData(GL_UNIFORM_BUFFER, maps.size()*4*_COLORS_PER_COLORMAP*sizeof(GLfloat), 0, GL_STATIC_DRAW);
	for (std::pair<std::string, std::pair<unsigned int, float*> > colorMap : maps) {
		glBufferSubData(GL_UNIFORM_BUFFER,  
				colorMap.second.first*4*_COLORS_PER_COLORMAP*sizeof(GLfloat), 
				4*_COLORS_PER_COLORMAP*sizeof(GLfloat), 
				colorMap.second.second);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
		
void OpenGLScene::changeColormap(const QString &colormapName) {
	std::map<std::string, std::pair<unsigned int, float*> > maps = ColorMap::multiHueColorMaps();
	m_colorId = maps[colormapName.toStdString()].first;
}


void OpenGLScene::renderString(float x, float y, void *font_, const char* string,
		float r, float g, float b, float a)
{  
  glColor4f(r,g,b,a); 
  glRasterPos2f(x, y);
  glutBitmapString(font_, (const unsigned char*)string);
}
