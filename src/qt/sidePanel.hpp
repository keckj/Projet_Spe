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
        std::map<std::string, bool> *getVariables();

        static const QStringList modelsList;

    signals:
        void startPushed();
        void pauseOrResumePushed(bool b);

        void childKeyEvent(QKeyEvent *k);

    public slots:
        void stop();

    private slots:
        void start_pause_resume();
        void openParametersDialog();
        void openInitDialog();
        void changeDirectory();
        void showSlider(int checkboxState);
        void changeNbIterSlider(int nbIter);
        void refreshParameters(int modelId);

    private:
        void setModelOptionsStatus(bool status);
        void keyPressEvent(QKeyEvent *k);

        bool m_paused;
        std::map<std::string, Argument> *m_argsMap;
        std::map<std::string, bool> *m_varsMap;
        //TODO initCond
        QString m_saveDirectory;

        ParametersDialog *initDialog, *paramsDialog;
        QComboBox *modelComboBox;
        QSpinBox *iterSpinBox;
        QListWidget *variablesRenderedList;
        QPushButton *initButton, *paramsButton, *saveDirButton, *startButton, *stopButton;
        QSlider *gridSlider;
};

#endif
