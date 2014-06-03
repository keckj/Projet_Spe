#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "headers.hpp"
#include "sidePanel.hpp"
#include <vector>
#include "grid2D.hpp"

class MainWindow : public QMainWindow {
	Q_OBJECT
	
	public:
		MainWindow();
		~MainWindow();

    signals:
        void textureUpdate(const Grid2D<float> *grid);
        void progressUpdate(int p);
		void colormapUpdate(unsigned int colorId);
        void pauseThread(bool b);
        void stopThread();

    public slots:
        void updateGrid(const Grid2D<float> *grid);
        void changeModel(int model);
        void changeNbIter(int nb);
        void startComputing();
        void pauseComputing(bool b);
        void stopComputing();
		void changeColormap(int colorId);
        void changeAutoRendering(int checkboxState);
		void changeDisplayedGrid(int n);

	private:
		void keyPressEvent(QKeyEvent *k);

        SidePanel *panel;

        QThread *m_thread;
        std::vector<Grid2D<float>> *m_stored_grids;
        float m_min_val, m_max_val;
        int m_selected_model;
        int m_total_steps;
        int m_displayed_grid;
        int m_auto_render;

	private slots:
		void childKeyEvent(QKeyEvent *k);
};





#endif
