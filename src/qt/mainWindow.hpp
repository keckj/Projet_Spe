#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "headers.hpp"
#include <vector>
#include "grid2D.hpp"
#include "computeThread.hpp"

class MainWindow : public QMainWindow {
	Q_OBJECT
	
	public:
		MainWindow();
		~MainWindow();

    signals:
        void textureUpdate(GLuint newTexture);
        void progressUpdate(int p);

    public slots:
        void updateGrid(const Grid2D<float> &grid);
        void changeModel(int model);
        void changeNbIter(int nb);
        void startComputing();
        void pauseComputing(); // todo in computeThread
        void stopComputing();
        void changeAutoRendering(int checkboxState);
        void changeDisplayedGrid(int n);

	private:
		void keyPressEvent(QKeyEvent *k);

        ComputeThread *m_thread;
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
