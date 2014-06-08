#include "headers.hpp"
#include "openGLScene.hpp"
#include "openGLScene.moc" 
#include "globals.hpp"
#include "colormap.hpp"

using namespace log4cpp;

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
        m_texMap = new QMap<QString, GLuint>;
		makeArrays();
		makeProgramm();
		makeColorMaps();
}

void OpenGLScene::textureUpdate(const Grid2D<float> *grid) {
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//float color[] = {1.0,1.0,1.0,1.0};
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, grid->width(), grid->height(), 0, GL_LUMINANCE, GL_FLOAT, (GLvoid*) grid->data());
}

void OpenGLScene::updateTextures(const QMap<QString, GLuint> &texMap) {
    //TODO new QGraphicsTextItem for each texture
   
    // Check if we need to change layout
    if (texMap.size() == m_texMap->size()) {
        // Notify the GUI that we have made progress
        emit stepRendered();
        return;
    }

    // Free the old map and copy the new map
    delete m_texMap;
    m_texMap = new QMap<QString, GLuint>(texMap);

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

	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_drawProgram->use();
	
	glUniform1i(m_drawProgramUniformLocationMap["colormapId"], m_colorId); //[0-11] pour le moment
	glUniform1f(m_drawProgramUniformLocationMap["minVal"], 0.0f);
	glUniform1f(m_drawProgramUniformLocationMap["maxVal"], 1.0f);
	
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glActiveTexture(GL_TEXTURE0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0 , Globals::projectionViewUniformBlock);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1 , m_colormapsUBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexCoordsVBO);           
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_QUADS, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0); 
	glDisable(GL_TEXTURE_2D);

	QTimer::singleShot(0, this, SLOT(update()));
}

void OpenGLScene::makeProgramm() {
	m_drawProgram = new Program("Draw texture");
	m_drawProgram->bindAttribLocations("0 1", "vertexPos texCoord");
	m_drawProgram->bindFragDataLocation(0, "out_colour");
	m_drawProgram->bindUniformBufferLocations("0 1", "projectionView colorMaps");

	m_drawProgram->attachShader(Shader("shaders/colormap/draw_vs.glsl", GL_VERTEX_SHADER));
	m_drawProgram->attachShader(Shader("shaders/colormap/draw_fs.glsl", GL_FRAGMENT_SHADER));

	m_drawProgram->link();
	m_drawProgramUniformLocationMap = m_drawProgram->getUniformLocationsMap("colormapId minVal maxVal" ,true);
}

// TODO: use layout to fill arrays
void OpenGLScene::makeArrays() {
	
    // Delete unused VBOs
    if (m_texCoordsVBO)
        glDeleteBuffers(1, &m_texCoordsVBO);
    if (m_vertexCoordsVBO)
        glDeleteBuffers(1, &m_vertexCoordsVBO);
    
    // TODO dynamic gen buffers according to m_nTexturesWidth & m_nTexturesHeight 
    float texCoords[] = {0,0,  0,1,  1,1,  1,0};
	float vertexCoords[] = {-1,1,  -1,-1,  1,-1,  1,1};

	glGenBuffers(1, &m_texCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
	//glBufferData(GL_ARRAY_BUFFER, 8*m_texMap->size()*sizeof(float), texCoords, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), texCoords, GL_STATIC_DRAW);

	glGenBuffers(1, &m_vertexCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexCoordsVBO);
	//glBufferData(GL_ARRAY_BUFFER, 8*m_texMap->size()*sizeof(float), vertexCoords, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), vertexCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void OpenGLScene::makeColorMaps() {
	std::map<std::string, std::pair<unsigned int, float*> > maps = ColorMap::multiHueColorMaps();

    glGenBuffers(1, &m_colormapsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_colormapsUBO);
    glBufferData(GL_UNIFORM_BUFFER, maps.size()*4*_COLOR_PER_COLORMAP*sizeof(GLfloat), 0, GL_STATIC_DRAW);
	for (std::pair<std::string, std::pair<unsigned int, float*> > colorMap : maps) {
		glBufferSubData(GL_UNIFORM_BUFFER,  
				colorMap.second.first*4*_COLOR_PER_COLORMAP*sizeof(GLfloat), 
				4*_COLOR_PER_COLORMAP*sizeof(GLfloat), 
				colorMap.second.second);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
		
void OpenGLScene::changeColormap(const QString &colormapName) {
	std::map<std::string, std::pair<unsigned int, float*> > maps = ColorMap::multiHueColorMaps();
	m_colorId = maps[colormapName.toStdString()].first;
}
