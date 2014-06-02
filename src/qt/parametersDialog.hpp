#ifndef PARAM_DIALOG_HPP
#define PARAM_DIALOG_HPP

#include "headers.hpp"

class ParametersDialog : public QDialog {
    Q_OBJECT

    public:
        ParametersDialog(QWidget *parent_ = 0);

    private slots:
        void okClicked();
        void resetClicked();
};

#endif
