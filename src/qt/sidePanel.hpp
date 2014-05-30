#ifndef SIDE_PANEL_H
#define SIDE_PANEL_H

#include "headers.hpp"

class SidePanel : public QWidget {
    Q_OBJECT

    public:
		SidePanel(QWidget *parent_ = 0);
		~SidePanel();

    public slots:
        void start_pause_resume();
        void stop();
        void showSlider(int checkboxState);

    private:
        void setModelOptionsStatus(bool status);

        bool m_paused;

        QComboBox *modelComboBox;
        QSpinBox *iterSpinBox;
        QPushButton *startButton, *stopButton;
        QSlider *gridSlider;
};

#endif
