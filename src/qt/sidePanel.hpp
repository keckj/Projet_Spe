#ifndef SIDE_PANEL_H
#define SIDE_PANEL_H

#include "headers.hpp"
#include "parametersDialog.hpp"
#include "argument.hpp"

class SidePanel : public QWidget {
    Q_OBJECT

    public:
		SidePanel(QWidget *parent_ = 0);
		~SidePanel();

        std::map<std::string, Argument> *getArguments();

    signals:
        void startPushed();
        void pauseOrResumePushed(bool b);

        void childKeyEvent(QKeyEvent *k);

    public slots:
        void stop();

    private slots:
        void start_pause_resume();
        void openParametersDialog();
        void changeDirectory();
        void showSlider(int checkboxState);
        void changeNbIterSlider(int nbIter);

    private:
        void setModelOptionsStatus(bool status);
        void keyPressEvent(QKeyEvent *k);

        bool m_paused;

        QString m_saveDirectory;

        ParametersDialog *paramsDialog;
        QComboBox *modelComboBox;
        QSpinBox *iterSpinBox;
        QPushButton *paramsButton, *saveDirButton, *startButton, *stopButton;
        QSlider *gridSlider;
};

#endif
