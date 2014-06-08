#include "headers.hpp"
#include "statusBar.hpp"
#include "statusBar.moc"


StatusBar::StatusBar(QWidget *parent_) : 
QStatusBar(parent_),
progressBar(0), message(0)
{
	progressBar = new QProgressBar(this);
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setValue(0);
	progressBar->setMaximumSize(200,20);

	message = new QLabel(this);
	message->setText("");
	message->setMaximumSize(300,20);

	this->addWidget(message,1);
	this->addWidget(progressBar,1);
}
	
StatusBar::~StatusBar() {
}

void StatusBar::progressUpdate(int p) {
    this->progressBar->setValue(p);
}
