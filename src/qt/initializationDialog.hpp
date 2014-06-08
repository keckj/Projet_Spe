#ifndef INIT_DIALOG_HPP
#define INIT_DIALOG_HPP

#include "headers.hpp"

class InitializationDialog : public QDialog {
    Q_OBJECT

    public:
        InitializationDialog(QWidget *parent_ = 0);

    signals:
        void gridSizeUpdate(unsigned int width, unsigned int height, unsigned int length);

    private slots:
        void okClicked();
        void resetClicked();

    private:
        QSpinBox *widthSpinBox, *heightSpinBox, *lengthSpinBox;
};

#endif
