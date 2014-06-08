#ifndef INIT_DIALOG_HPP
#define INIT_DIALOG_HPP

#include "headers.hpp"

class InitializationDialog : public QDialog {
    Q_OBJECT

    public:
        InitializationDialog(unsigned int *gridWidth, unsigned int *gridHeight, unsigned int *gridLength, QWidget *parent_ = 0);
        ~InitializationDialog();

    private slots:
        void okClicked();
        void resetClicked();
        void changeGridDimension(int checkboxState);

    private:
        QCheckBox *grid3DCheckbox;
        QSpinBox *widthSpinBox, *heightSpinBox, *lengthSpinBox;

        unsigned int *m_gridWidth, *m_gridHeight, *m_gridLength;
};

#endif
