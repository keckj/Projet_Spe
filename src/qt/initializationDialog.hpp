#ifndef INIT_DIALOG_HPP
#define INIT_DIALOG_HPP

#include "headers.hpp"

class InitializationDialog : public QDialog {
    Q_OBJECT

    public:
        InitializationDialog(std::map<QString, bool> *varsMap,
                             std::map<QString, int> *condsMap,
                             std::map<QString, int> *defaultCondsMap,
                             unsigned int *gridWidth,
                             unsigned int *gridHeight, 
                             unsigned int *gridLength, 
                             QWidget *parent_ = 0);
        ~InitializationDialog();

    private slots:
        void okClicked();
        void resetClicked();
        void changeGridDimension(int checkboxState);

    private:
        QCheckBox *grid3DCheckbox;
        QSpinBox *widthSpinBox, *heightSpinBox, *lengthSpinBox;
        std::map<QString, QComboBox *> *m_widgetsMap;

        std::map<QString, int> *m_condsMap;
        std::map<QString, int> *m_defaultCondsMap;
        unsigned int *m_gridWidth, *m_gridHeight, *m_gridLength;
};

#endif
