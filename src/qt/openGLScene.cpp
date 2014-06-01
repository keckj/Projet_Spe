#include "headers.hpp"
#include "openGLScene.hpp"
#include "openGLScene.moc" 

OpenGLScene::OpenGLScene() {

    //TODO: add widgets here
}

void OpenGLScene::textureUpdate(GLuint newTexture) {
    currentTexture = newTexture;
}

void OpenGLScene::drawBackground(QPainter *painter, const QRectF &) {

    glClearColor(0.2,0.2,0.2,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, currentTexture);

    glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex2f(-1,1);
    glTexCoord2f(0,1);
    glVertex2f(-1,-1);
    glTexCoord2f(1,1);
    glVertex2f(1,-1);
    glTexCoord2f(1,0);
    glVertex2f(1,1);
    glEnd();
   
    glBindTexture(GL_TEXTURE_2D, 0); 
    glDisable(GL_TEXTURE_2D);

    QTimer::singleShot(0, this, SLOT(update()));
}
    
