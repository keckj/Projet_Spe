
#ifndef MENUBAR_H
#define MENUBAR_H

#include "headers.hpp"

class MenuBar : public QMenuBar {
	
	public:
		MenuBar(QWidget *parent_ = 0);

	private:
		QAction *quit;
		QMenu *file;
};

#endif /* end of include guard: MENUBAR_H */
