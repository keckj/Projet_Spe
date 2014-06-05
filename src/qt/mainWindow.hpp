#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "headers.hpp"
#include "sidePanel.hpp"
#include <vector>
#include "grid2D.hpp"

Q_DECLARE_METATYPE(std::string)

class MainWindow : public QMainWindow {
	Q_OBJECT
	
	public:
		MainWindow();
		~MainWindow();

    signals:
        void textureUpdate(const Grid2D<float> *grid);
        void progressUpdate(int p);
        void addTextureRequest(std::string);
        void removeTextureRequest(std::string);
		void colormapUpdate(const QString &colormapName);
        void pauseThread(bool b);
        void stopThread();

    public slots:
        void updateGrid(const Grid2D<float> *grid);
        //void updateTextures(ptr);
        void changeModel(int model);
        void changeNbIter(int nb);
        void startComputing();
        void pauseComputing(bool b);
        void stopComputing();
        void updateRenderedVars(QListWidgetItem *item);
		void changeColormap(const QString &colormapName);
        void changeAutoRendering(int checkboxState);
		void changeDisplayedGrid(int n);

	private:
		void keyPressEvent(QKeyEvent *k);

        SidePanel *panel;

        QThread *m_thread;
        std::vector<Grid2D<float>> *m_stored_grids;
        std::map<std::string, GLuint> *m_texturesToRender;
        float m_min_val, m_max_val;
        int m_selected_model;
        int m_total_steps;
        int m_displayed_grid;
        int m_auto_render;

	private slots:
		void childKeyEvent(QKeyEvent *k);
};





#endif
