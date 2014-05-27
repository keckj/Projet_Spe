#include "../utils/headers.hpp"
#include "mainWindow.hpp"
#include "mainWindow.moc"
#include "../grid.hpp"
#include "../computeThread.hpp"

MainWindow::MainWindow() /*:
    //slider(0), voxelViewer(0), sidePanel(0), menuBar(0), statusBar(0) */{

        // Thread
        thread = new ComputeThread();
        connect(thread, SIGNAL(stepComputed(Grid)), this, SLOT(updateGrid(Grid)));
		
        
        // GUI
        //qtgui::mainWindow = this;
	
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

		splitter = new QSplitter(Qt::Horizontal, this);
		
		viewer = new Viewer();
		sidePanel = new SidePanel(slider);

		connect(sidePanel, SIGNAL(draw()), viewer, SLOT(draw()));
		
		connect(viewer, SIGNAL(childKeyEvent(QKeyEvent *)), this, SLOT(childKeyEvent(QKeyEvent *)));

		slider->addWidget(viewer);
		slider->addWidget(sidePanel);
		slider->setStretchFactor(0,6);
		slider->setStretchFactor(1,3);

		this->setCentralWidget(slider);*/
		this->show();
	}

MainWindow::~MainWindow() {
}

void MainWindow::updateGrid(const Grid &grid) {
    //TODO
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
