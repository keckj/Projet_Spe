#include "headers.hpp"
#include "openGLScene.hpp"
#include "openGLScene.moc" 
#include "globals.hpp"

OpenGLScene::OpenGLScene() :
	m_drawProgram(0),
	m_currentTexture(0),
	m_texCoordsVBO(0),
	m_vertexCoordsVBO(0),
	m_texture(0)
{
}

void OpenGLScene::textureUpdate(const Grid2D<float> *grid) {
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, grid->width(), grid->height(), 0, GL_LUMINANCE, GL_FLOAT, (GLvoid*) grid->data());
}

void OpenGLScene::drawBackground(QPainter *painter, const QRectF &) {

	glClearColor(0.2,0.2,0.2,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(m_drawProgram == 0) {
		makeArrays();
		makeProgramm();
	}

	m_drawProgram->use();

	glEnable(GL_TEXTURE_2D);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0 , Globals::projectionViewUniformBlock);

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
	m_drawProgram->bindUniformBufferLocations("0", "projectionView");

	m_drawProgram->attachShader(Shader("shaders/colormap/draw_vs.glsl", GL_VERTEX_SHADER));
	m_drawProgram->attachShader(Shader("shaders/colormap/draw_fs.glsl", GL_FRAGMENT_SHADER));

	m_drawProgram->link();
}

void OpenGLScene::makeArrays() {
	float texCoords[] = {0,0,  0,1,  1,1,  1,0};
	float vertexCoords[] = {-1,1,  -1,-1,  1,-1,  1,1};

	glGenBuffers(1, &m_texCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), texCoords, GL_STATIC_DRAW);

	glGenBuffers(1, &m_vertexCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), vertexCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


