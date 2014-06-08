#ifndef OPENGL_SCENE_HPP
#define OPENGL_SCENE_HPP

#include "headers.hpp"
#include "program.hpp"
#include "texture.hpp"
#include "grid2D.hpp"

#include <mutex>
#include <condition_variable>

class OpenGLScene : public QGraphicsScene {
    Q_OBJECT

    public:
        OpenGLScene();

        void drawBackground(QPainter *painter, const QRectF &);
		
		static std::mutex glMutex;
		static std::condition_variable glConditionVariable;

		static GLXContext glContext;
		static Display *glDisplay;
	
    //signals:
    //    void progressUpdate(int p);

    public slots:
        void textureUpdate(const Grid2D<float> *grid);
        //void textureUpdate(const QMap<QString, GLuint> &texMap);
		void changeColormap(const QString &colormapName);

    private:
		Program *m_drawProgram;
		std::map<std::string, int> m_drawProgramUniformLocationMap;
		Texture *m_currentTexture;
		unsigned int m_texCoordsVBO;
		unsigned int m_vertexCoordsVBO;
		unsigned int m_texture;
		unsigned int m_colormapsUBO;
		unsigned int m_colorId;
		
		void makeProgramm();
		void makeColorMaps();
		void makeArrays();

};
#endif

