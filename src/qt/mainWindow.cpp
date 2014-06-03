#include "headers.hpp"
#include <vector>
#include <cfloat>
#include "mainWindow.hpp"
#include "mainWindow.moc"
#include "menuBar.hpp"
#include "statusBar.hpp"
#include "sidePanel.hpp"
#include "graphicsViewer.hpp"
#include "openGLScene.hpp"
#include "grid2D.hpp"
#include "log.hpp"
// Models
#include "model.hpp"
#include "exampleModel.hpp"
#include "simpleModel2D.hpp"
#include "multigpu.hpp"

MainWindow::MainWindow() {

    // Grids
    m_stored_grids = new std::vector<Grid2D<float>>();
    m_min_val = FLT_MAX; m_max_val = -FLT_MAX;
    m_total_steps = 100;
    m_auto_render = true;

    // QT GUI
    QDesktopWidget widget;
    QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen());

    this->setWindowTitle("Electrophysiological models simulator");
    this->resize(mainScreenSize.width()*0.75,mainScreenSize.height()*0.75);
    this->setStyleSheet("QMainWindow { background-color: white; }");
    this->setAutoFillBackground(true);

    MenuBar *menu = new MenuBar(this);
    this->setMenuBar(menu);

    StatusBar *status = new StatusBar(this);
    this->setStatusBar(status);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    QGLWidget *qglwidget =  new QGLWidget(QGLFormat(QGL::SampleBuffers));
    qglwidget->makeCurrent();

    GraphicsViewer *viewer = new GraphicsViewer();
    viewer->setViewport(qglwidget);
    viewer->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    OpenGLScene *scene = new OpenGLScene();
    viewer->setScene(scene);

    panel = new SidePanel(this);

    connect(panel, SIGNAL(childKeyEvent(QKeyEvent *)), this, SLOT(childKeyEvent(QKeyEvent *)));
    
    connect(this, SIGNAL(textureUpdate(const Grid2D<float> *)), scene, SLOT(textureUpdate(const Grid2D<float> *)));
    connect(this, SIGNAL(progressUpdate(int)), status, SLOT(progressUpdate(int)));
	connect(this, SIGNAL(colormapUpdate(unsigned int)), scene, SLOT(changeColormap(unsigned int)));

    splitter->addWidget(viewer);
    splitter->addWidget(panel);
    splitter->setStretchFactor(0,1);
    splitter->setStretchFactor(1,0);

    this->setCentralWidget(splitter);
    this->show();
}

MainWindow::~MainWindow() {
}

void MainWindow::updateGrid(const Grid2D<float> *grid) {
    // Add grid to the list of stored grids
    m_stored_grids->push_back(*grid); 

    // Create 2D texture
    //GLuint texture;
    //glGenTextures(1, &texture);
    //glBindTexture(GL_TEXTURE_2D, texture);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
	//float *gridData = grid->data();
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, grid->width(), grid->height(), 0, GL_LUMINANCE, GL_FLOAT, (GLvoid*) gridData);

    // Update displayed grid if auto rendering
    //if (m_auto_render)
        //m_displayed_grid = texture;

    // Tell the scene to change the texture it's using
    emit textureUpdate(grid);

    // Update progress bar TODO: * nb_it to render
    emit progressUpdate((float) m_stored_grids->size() / m_total_steps * 100);
}

void MainWindow::changeModel(int model) {
    m_selected_model = model;
}

void MainWindow::changeNbIter(int nb) {
    m_total_steps = nb;
}

void MainWindow::startComputing() {
    m_stored_grids->clear();
    m_thread = new QThread;
    Model *mod;
    switch (m_selected_model) {
		case 1:
            mod = (Model *) new SimpleModel2D(m_total_steps);
			log_console->infoStream() << "Started a simple model 2D simulation !";
			break;
		case 2:
            mod = (Model *) new MultiGpu(m_total_steps);
			log_console->infoStream() << "Started a yolo-swaggy bug multi-gpu model simulation !";
			break;
        default:
            mod = (Model *) new ExampleModel(m_total_steps);
			log_console->infoStream() << "Started an example model simulation !";
    }

    mod->moveToThread(m_thread);
    //connect(mod, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(m_thread, SIGNAL(started()), mod, SLOT(startComputing()));
    connect(this, SIGNAL(pauseThread(bool)), mod, SLOT(pauseComputing(bool)));
    connect(mod, SIGNAL(finished()), m_thread, SLOT(quit()));                   // kill thread
    connect(mod, SIGNAL(finished()), panel, SLOT(stop()));                      // update GUI buttons
    //connect(mod, SIGNAL(finished()), mod, SLOT(deleteLater()));
    connect(m_thread, SIGNAL(finished()), m_thread, SLOT(deleteLater()));       // cleanup
    connect(mod, SIGNAL(stepComputed(const Grid2D<float> *)), this, SLOT(updateGrid(const Grid2D<float> *)));

    m_thread->start();
}

void MainWindow::pauseComputing(bool b) {
    emit pauseThread(b);
}

void MainWindow::stopComputing() {
    emit stopThread();
}

void MainWindow::changeColormap(int colorId) {
	emit colormapUpdate(colorId);
}

void MainWindow::changeAutoRendering(int checkboxState) {
    m_auto_render = (checkboxState > 0);
}

void MainWindow::changeDisplayedGrid(int n) {
	m_displayed_grid = n;
	//emit textureUpdate(m_displayed_grid);
}


void MainWindow::keyPressEvent(QKeyEvent *k) {

    switch (k->key()) {
        case Qt::Key_Escape:
            if (m_thread)
               m_thread->quit(); 
            this->close();
            break;
    }
}

void MainWindow::childKeyEvent(QKeyEvent *k) {
    keyPressEvent(k);
}
		

