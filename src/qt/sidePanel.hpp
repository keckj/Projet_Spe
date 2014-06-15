#ifndef SIDE_PANEL_H
#define SIDE_PANEL_H

#include "headers.hpp"
#include "parametersDialog.hpp"
#include "initializationDialog.hpp"
#include "argument.hpp"
#include "initialCond.hpp"

class SidePanel : public QWidget {
    Q_OBJECT

    public:
        SidePanel(QWidget *parent_ = 0);
        ~SidePanel();

        unsigned int getGridWidth();
        unsigned int getGridHeight();
        unsigned int getGridLength();
        std::map<QString, Argument> *getArguments();
        std::map<QString, bool> *getVariables();
        std::map<QString, int> *getInitialConditions();
        QString getSaveDirectory();

        static const QStringList modelsList;
        static const QStringList initialConditionsList;
        static const int defaultNumberOfSteps;
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
        void updateRenderedVars(QListWidgetItem *item);

    private:
        void setModelOptionsStatus(bool status);
        void keyPressEvent(QKeyEvent *k);

        bool m_paused;
        unsigned int m_gridWidth, m_gridHeight, m_gridLength;
        std::map<QString, Argument> *m_argsMap;
        std::map<QString, bool> *m_varsMap;
        std::map<QString, int> *m_initialCondsMap;
        std::map<QString, int> *m_defaultCondsMap;
        QString m_saveDirectory;

        InitializationDialog *initDialog;
        ParametersDialog *paramsDialog;
        QComboBox *modelComboBox;
        QSpinBox *iterSpinBox;
        QCheckBox *saveDirCheckBox;
        QListWidget *variablesRenderedList;
        QPushButton *initButton, *paramsButton, *saveDirButton, *startButton, *stopButton;
        QSlider *gridSlider;
};

#endif
