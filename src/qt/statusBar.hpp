#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "headers.hpp"

class StatusBar : public QStatusBar {
    Q_OBJECT
    
	public:
		StatusBar(QWidget *parent_ = 0);
		~StatusBar();

    public slots:
        void progressUpdate(int p);

	private:
		QProgressBar *progressBar;
		QLabel *message;
};


#endif /* end of include guard: STATUSBAR_H */
