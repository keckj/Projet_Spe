#ifndef OPENGL_SCENE_HPP
#define OPENGL_SCENE_HPP

#include "headers.hpp"

class OpenGLScene : public QGraphicsScene {
    Q_OBJECT

    public:
        OpenGLScene();

        void drawBackground(QPainter *painter, const QRectF &);

    private:
        GLuint currentTexture;

    public slots:
        void textureUpdate(GLuint newTexture);
};
#endif

