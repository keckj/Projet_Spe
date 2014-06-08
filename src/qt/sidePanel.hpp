#ifndef SIDE_PANEL_H
#define SIDE_PANEL_H

#include "headers.hpp"
#include "parametersDialog.hpp"
#include "initializationDialog.hpp"
#include "argument.hpp"

class SidePanel : public QWidget {
    Q_OBJECT

    public:
        SidePanel(QWidget *parent_ = 0);
        ~SidePanel();

        unsigned int getGridWidth();
        unsigned int getGridHeight();
        unsigned int getGridLength();
        std::map<std::string, Argument> *getArguments();
        std::map<std::string, bool> *getVariables();

        static const QStringList modelsList;
        static const std::vector<unsigned int> defaultGridSize;

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
        unsigned int m_gridWidth, m_gridHeight, m_gridLength;
        std::map<std::string, Argument> *m_argsMap;
        std::map<std::string, bool> *m_varsMap;
        //TODO initCond
        QString m_saveDirectory;

        InitializationDialog *initDialog;
        ParametersDialog *paramsDialog;
        QComboBox *modelComboBox;
        QSpinBox *iterSpinBox;
        QListWidget *variablesRenderedList;
        QPushButton *initButton, *paramsButton, *saveDirButton, *startButton, *stopButton;
        QSlider *gridSlider;
};

#endif
