#ifndef PARAM_DIALOG_HPP
#define PARAM_DIALOG_HPP

#include "headers.hpp"
#include "argument.hpp"

class ParametersDialog : public QDialog {
    Q_OBJECT

    public:
        ParametersDialog(QWidget *parent_ = 0);

    private slots:
        void okClicked();
        void resetClicked();

    private:
        std::map<std::string, Argument> *m_argsMap;
};

#endif
