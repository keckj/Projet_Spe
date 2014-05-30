#include "headers.hpp"
#include "menuBar.hpp"

MenuBar::MenuBar(QWidget *parent_) : QMenuBar(parent_) {
		file = this->addMenu("&File");
		quit = new QAction("&Quit", this);
		
		file->addAction(quit);
		connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
}
