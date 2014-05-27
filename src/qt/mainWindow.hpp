#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "../utils/headers.hpp"
#include "../grid.hpp"
#include "../computeThread.hpp"

class MainWindow : public QMainWindow {
	Q_OBJECT
	
	public:
		MainWindow();
		~MainWindow();

	private:
        //QSplitter *splitter;
		//Viewer *viewer;
		//SidePanel *sidePanel;
		//MenuBar *menuBar;
		//StatusBar *statusBar;
        ComputeThread *thread;

		void keyPressEvent(QKeyEvent *k);
	
	private slots:
		void childKeyEvent(QKeyEvent *k);
        void updateGrid(const Grid &grid);
};





#endif
