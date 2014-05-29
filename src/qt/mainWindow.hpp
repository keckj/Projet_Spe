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

	private:
        ComputeThread *m_thread;
        std::vector<Grid2D<float>> *m_stored_grids;
        float m_min_val, m_max_val;

		void keyPressEvent(QKeyEvent *k);

	private slots:
		void childKeyEvent(QKeyEvent *k);
        void updateGrid(const Grid2D<float> &grid);
};





#endif
