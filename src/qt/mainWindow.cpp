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

    // Simulation
    //m_stored_grids = new std::vector<Grid2D<float>>();
    m_selected_model = 0;
    m_current_step = 0;
    m_total_steps = SidePanel::defaultNumberOfSteps;
    m_auto_render = true;

	qRegisterMetaType< QMap<QString, GLuint> >("QMap<QString, GLuint>");

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
    scene = new OpenGLScene(viewer);
    viewer->setScene(scene);
    
    connect(scene, SIGNAL(stepRendered()), this, SLOT(onStepRender()));

    panel = new SidePanel(this);

    connect(panel, SIGNAL(childKeyEvent(QKeyEvent *)), this, SLOT(childKeyEvent(QKeyEvent *)));
    
    connect(this, SIGNAL(progressUpdate(int)), status, SLOT(progressUpdate(int)));
	connect(this, SIGNAL(colormapUpdate(const QString &)), scene, SLOT(changeColormap(const QString &)));

    splitter->addWidget(viewer);
    splitter->addWidget(panel);
    splitter->setStretchFactor(0,1);
    splitter->setStretchFactor(1,0);

    this->setCentralWidget(splitter);
    this->show();
}

MainWindow::~MainWindow() {
}


void MainWindow::changeModel(int model) {
    m_selected_model = model;
}

void MainWindow::changeNbIter(int nb) {
    m_total_steps = nb;
}

void MainWindow::startComputing() {
    //m_stored_grids->clear();
    
    unsigned int gridWidth = panel->getGridWidth();
    unsigned int gridHeight = panel->getGridHeight();
    unsigned int gridLength = panel->getGridLength();

    //TODO: panel->getVariables(), panel->getArguments() for every model
    //TODO: use panel->getSaveDirectory()
    //TODO: use panel->getInitialConditions()

    m_thread = new QThread;
    Model *mod;
    switch (m_selected_model) {
		case 0:
            mod = (Model *) new SimpleModel2D(m_total_steps, panel->getArguments(), panel->getVariables(), gridWidth, gridHeight);
			log_console->infoStream() << "Started a simple model 2D simulation !";
			break;
		case 1:
            mod = (Model *) new MultiGpu(m_total_steps, panel->getVariables());
			log_console->infoStream() << "Started a yolo-swaggy bug multi-gpu model simulation !";
			break;
        default:
            mod = (Model *) new ExampleModel(m_total_steps, panel->getVariables());
			log_console->infoStream() << "Started an example model simulation !";
    }
    mod->moveToThread(m_thread);
    
    connect(m_thread, SIGNAL(started()), mod, SLOT(startComputing()));
    connect(this, SIGNAL(pauseThread(bool)), mod, SLOT(pauseComputing(bool)));
    connect(mod, SIGNAL(finished()), m_thread, SLOT(quit()));                   // kill thread
    connect(mod, SIGNAL(finished()), panel, SLOT(stop()));                      // update GUI buttons
    connect(mod, SIGNAL(finished()), mod, SLOT(deleteLater()));
    connect(m_thread, SIGNAL(finished()), m_thread, SLOT(deleteLater()));
   
	connect(mod, SIGNAL(stepComputed(const QMap<QString, GLuint> &)), scene, SLOT(updateTextures(const QMap<QString, GLuint> &)));

    m_thread->start();

    // Reset current step and progress bar
    m_current_step = 0;
    emit progressUpdate(0);
}

void MainWindow::pauseComputing(bool b) {
    emit pauseThread(b);
}

void MainWindow::stopComputing() {
    emit stopThread();
}
       
void MainWindow::onStepRender() {
    m_current_step++;
    emit progressUpdate((float) m_current_step / m_total_steps * 100);
}

void MainWindow::changeColormap(const QString &colormapName) {
	emit colormapUpdate(colormapName);
}

void MainWindow::changeAutoRendering(int checkboxState) {
    m_auto_render = (checkboxState > Qt::Unchecked);
}

void MainWindow::changeDisplayedGrid(int n) {
	m_displayed_grid = n;
	//emit textureUpdate(m_displayed_grid);
}


void MainWindow::keyPressEvent(QKeyEvent *k) {
    switch (k->key()) {
        case Qt::Key_Escape:
            emit stopThread();
            this->close();
            break;
    }
}

void MainWindow::childKeyEvent(QKeyEvent *k) {
    keyPressEvent(k);
}
		

