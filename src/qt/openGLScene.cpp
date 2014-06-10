#include "headers.hpp"
#include "openGLScene.hpp"
#include "openGLScene.moc" 
#include "globals.hpp"
#include "colormap.hpp"
		
std::mutex OpenGLScene::glMutex;
std::condition_variable OpenGLScene::glConditionVariable;

using namespace log4cpp;

OpenGLScene::OpenGLScene(GraphicsViewer *viewer) :
	QGraphicsScene(),
    m_viewer(viewer),
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
        m_textItemsVec = new QVector<QGraphicsTextItem *>;
		makeArrays();
		makeProgram();
		makeColorMaps();
}

void OpenGLScene::textureUpdate(const Grid2D<float> *grid) {
	std::unique_lock<std::mutex> lock(glMutex);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, grid->width(), grid->height(), 0, GL_LUMINANCE, GL_FLOAT, (GLvoid*) grid->data());

    //test DOESN'T WORK :(
    if (m_textItemsVec->size() == 0) {
        float deltaW = 2.0; 
        float deltaH = 2.0;
        float x, y;
        //QGraphicsTextItem *textItem = this->addText("");
        addWidget(new QLabel("TEST"))->setZValue(2);
        QGraphicsTextItem *textItem = new QGraphicsTextItem("TEST");
        //textItem->setDefaultTextColor(Qt::red);
        //textItem->setHtml("<div style='background-color:rgb(0,0,0);'>" + QString("TEST") + "</div>");
        // Map GL coordinates to scene coordinates
        //QPointF p = m_viewer->mapToScene(-1 + (0+0.5)*deltaW, 1 - 0*deltaH - 0.1);
        //x = p.x(); y = p.y();
        x = -1+0.5+deltaW; y = 1 - 0.1;
        // Map coordinates to top corner coordinates
        //x = x - textItem->boundingRect().width()  / 2;
        //y = y - textItem->boundingRect().height() / 2;
        //this->addWidget(new QLabel("TESTTTT"))->setPos(100,100);
        textItem->setPos(x, y);
        this->addItem(textItem);
        setSceneRect(-1,-1,2,2);
        std::cout << x << " " << y << std::endl;
        m_textItemsVec->push_back(textItem);
        std::cout << itemsBoundingRect().width() << " " << itemsBoundingRect().height() << std::endl;
        std::cout << sceneRect().width() <<  " " << sceneRect().height() << std::endl;
    }

	glConditionVariable.notify_one();
}

void OpenGLScene::updateTextures(const QMap<QString, GLuint> &texMap) {
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
    
    // Delete all previous text items
    this->clear();

    //Add a QGraphicsTextItem for each texture
    QGraphicsTextItem *textItem;
    int w = 0;
    int h = 0;
    float deltaW = 2.0 / m_nTexturesWidth; 
    float deltaH = 2.0 / m_nTexturesHeight;
    int x, y;
    for (auto it = m_texMap->constBegin(); it != m_texMap->constEnd(); ++it) {
        textItem = this->addText("");
        textItem->setDefaultTextColor(Qt::white);
        textItem->setHtml("<div style='background-color:rgb(0,0,0);'>" + it.key() + "</div>");
        // Map GL coordinates to scene coordinates
        QPointF p = m_viewer->mapToScene(-1 + (w+0.5)*deltaW, 1 - h*deltaH - 0.1);
        x = p.x(); y = p.y();
        // Map coordinates to top corner coordinates
        x = x - textItem->boundingRect().width()  / 2;
        y = y - textItem->boundingRect().height() / 2;
        textItem->setPos(x, y);
        w++;
        h++;
    }

    // Notify the status bar that we have made progress
    emit stepRendered();
}

void OpenGLScene::drawBackground(QPainter *painter, const QRectF &) {
	// Un lock dans la main loop de draw ???
    std::unique_lock<std::mutex> lock(glMutex);

	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_drawProgram->use();
	
	glUniform1i(m_drawProgramUniformLocationMap["colormapId"], m_colorId);
	glUniform1f(m_drawProgramUniformLocationMap["minVal"], 0.0f);
	glUniform1f(m_drawProgramUniformLocationMap["maxVal"], 1.0f);
	
    glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0 , Globals::projectionViewUniformBlock);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1 , m_colormapsUBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexCoordsVBO);           
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

    /*int idx = 0; // starting index in the VBOs
    for (auto it = m_texMap->constBegin(); it < m_texMap->constEnd(); ++it) {
	    glBindTexture(GL_TEXTURE_2D, it.value());
	    glDrawArrays(GL_QUADS, idx, 4);
        idx += 4;
    }*/

	glBindTexture(GL_TEXTURE_2D, m_texture);
    glDrawArrays(GL_QUADS, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0); 
	glDisable(GL_TEXTURE_2D);

	QTimer::singleShot(0, this, SLOT(update()));
	
	glConditionVariable.notify_one();
}

void OpenGLScene::makeProgram() {
	std::unique_lock<std::mutex> lock(glMutex);
	m_drawProgram = new Program("Draw texture");
	m_drawProgram->bindAttribLocations("0 1", "vertexPos texCoord");
	m_drawProgram->bindFragDataLocation(0, "out_colour");
	m_drawProgram->bindUniformBufferLocations("0 1", "projectionView colorMaps");

	m_drawProgram->attachShader(Shader("shaders/colormap/draw_vs.glsl", GL_VERTEX_SHADER));
	m_drawProgram->attachShader(Shader("shaders/colormap/draw_fs.glsl", GL_FRAGMENT_SHADER));

	m_drawProgram->link();
	m_drawProgramUniformLocationMap = m_drawProgram->getUniformLocationsMap("colormapId minVal maxVal" ,true);
	
	glConditionVariable.notify_one();
}
        
void OpenGLScene::makeArrays() {
	std::unique_lock<std::mutex> lock(glMutex);

    float texCoords[] = {0,0,  0,1,  1,1,  1,0};
	float vertexCoords[] = {-1,1,  -1,-1,  1,-1,  1,1};
    
	// Delete unused VBOs
    //if (m_texCoordsVBO)
        //glDeleteBuffers(1, &m_texCoordsVBO);
    //if (m_vertexCoordsVBO)
        //glDeleteBuffers(1, &m_vertexCoordsVBO);
    
    // TODO dynamic gen buffers according to m_nTexturesWidth & m_nTexturesHeight 
    /*
    float vertexCoords[8*m_texMap->size()];
    float texCoords[8*m_texMap->size()];
    float margin = 0.01; //TODO
    float deltaW = 2.0 / m_nTexturesWidth; 
    float deltaH = 2.0 / m_nTexturesHeight;

    int idx = 0;
    // Fill the grid by lines first
    for (int h = 0; h < m_nTexturesHeight; h++) {
        for (int w = 0; w < m_nTexturesWidth; w++) {
            // Do not draw a quad if there is no texture left
            if (idx == 8*m_texMap->size()) {
                break;
            }
            // 1st point (top left)
            vertexCoords[idx] = -1 + w*deltaW    ; texCoords[idx++] = 0;
            vertexCoords[idx] =  1 - h*deltaH    ; texCoords[idx++] = 0;
            // 2nd point (bottom left)
            vertexCoords[idx] = -1 + w*deltaW    ; texCoords[idx++] = 0;
            vertexCoords[idx] =  1 - (h+1)*deltaH; texCoords[idx++] = 1;
            // 3rd point (bottom right)
            vertexCoords[idx] = -1 + (w+1)*deltaW; texCoords[idx++] = 1;
            vertexCoords[idx] =  1 - (h+1)*deltaH; texCoords[idx++] = 1;
            // 4th point (top right)
            vertexCoords[idx] = -1 + (w+1)*deltaW; texCoords[idx++] = 1;
            vertexCoords[idx] =  1 - h*deltaH    ; texCoords[idx++] = 0;
        }
    }
    */

	glGenBuffers(1, &m_texCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
	//glBufferData(GL_ARRAY_BUFFER, 8*m_texMap->size()*sizeof(float), texCoords, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), texCoords, GL_STATIC_DRAW);

	glGenBuffers(1, &m_vertexCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexCoordsVBO);
	//glBufferData(GL_ARRAY_BUFFER, 8*m_texMap->size()*sizeof(float), vertexCoords, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), vertexCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glConditionVariable.notify_one();
}


void OpenGLScene::makeColorMaps() {
	std::unique_lock<std::mutex> lock(glMutex);

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
	
	glConditionVariable.notify_one();
}
		
void OpenGLScene::changeColormap(const QString &colormapName) {
	std::map<std::string, std::pair<unsigned int, float*> > maps = ColorMap::multiHueColorMaps();
	m_colorId = maps[colormapName.toStdString()].first;
}
