#include "headers.hpp"
#include <vector>
#include <cfloat>
#include "mainWindow.hpp"
#include "mainWindow.moc"
#include "graphicsViewer.hpp"
#include "openGLScene.hpp"
#include "grid2D.hpp"
#include "computeThread.hpp"

MainWindow::MainWindow() {

    // Thread
    m_thread = new ComputeThread();
    connect(m_thread, SIGNAL(stepComputed(const Grid2D<float> &)), this, SLOT(updateGrid(const Grid2D<float> &)));

    // Grids
    m_stored_grids = new std::vector<Grid2D<float>>();
    m_min_val = FLT_MAX; m_max_val = -FLT_MAX;

    // QT GUI
    QDesktopWidget widget;
    QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen());

    this->setWindowTitle("Ma premiere application QT :D");
    this->resize(mainScreenSize.width()/2,mainScreenSize.height()/2);
    this->setStyleSheet("QMainWindow { background-color: white; }");
    this->setAutoFillBackground(true);

    /*menuBar = new MenuBar(this);
      this->setMenuBar(menuBar);

      statusBar = new StatusBar(this);
      this->setStatusBar(statusBar);
      */
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    QGLWidget *qglwidget =  new QGLWidget(QGLFormat(QGL::SampleBuffers));
    qglwidget->makeCurrent();

    GraphicsViewer *viewer = new GraphicsViewer();
    viewer->setViewport(qglwidget);
    viewer->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    OpenGLScene *scene = new OpenGLScene();
    viewer->setScene(scene);

    /*sidePanel = new SidePanel(slider);

      connect(sidePanel, SIGNAL(draw()), viewer, SLOT(draw()));

      connect(viewer, SIGNAL(childKeyEvent(QKeyEvent *)), this, SLOT(childKeyEvent(QKeyEvent *)));
      */
    connect(this, SIGNAL(textureUpdate(GLuint)), scene, SLOT(textureUpdate(GLuint)));

    splitter->addWidget(viewer);
    //splitter->addWidget(sidePanel);
    splitter->setStretchFactor(0,6);
    splitter->setStretchFactor(1,3);

    this->setCentralWidget(splitter);
    //this->setCentralWidget(viewer);
    this->show();

    //hack
    Grid2D<float> g2d(1.0,1.0,100u,100u,true);
    this->updateGrid(g2d);
}

MainWindow::~MainWindow() {
}

void MainWindow::updateGrid(const Grid2D<float> &grid) {
    std::cout << grid << std::endl;
    // Add grid to the list of stored grids
    m_stored_grids->push_back((Grid2D<float>) grid); 

    // Update val_min and val_max
    for (unsigned int j = 0; j < grid.height(); j++) {
        for (unsigned int i = 0; i < grid.width(); i++) {
            float val = grid(i,j);
            if (val < m_min_val) m_min_val = val;
            if (val > m_max_val) m_max_val = val;
        }
    }

    // Create 2D texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //hack
    float *gridData = new float[grid.width()*grid.height()];
    for (unsigned int j = 0; j < grid.height(); j++) {
        for (unsigned int i = 0; i < grid.width(); i++) {
            gridData[j*grid.width()+i] = (2*i < grid.width()) ? 0.0f : 1.0f;
        }
    }
    //float *gridData = grid.data();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, grid.width(), grid.height(), 0, GL_LUMINANCE, GL_FLOAT, (GLvoid*) gridData);

    // Tell the scene to change the texture it's using
    emit textureUpdate(texture);
}

void MainWindow::keyPressEvent(QKeyEvent *k) {

    switch(k->key()) {
        case Qt::Key_Escape:
            this->close();
            break;
    }
}

void MainWindow::childKeyEvent(QKeyEvent *k) {
    keyPressEvent(k);
}
