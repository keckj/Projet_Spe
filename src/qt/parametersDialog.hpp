#ifndef PARAM_DIALOG_HPP
#define PARAM_DIALOG_HPP

#include "headers.hpp"
#include "argument.hpp"

class ParametersDialog : public QDialog {
    Q_OBJECT

    public:
        ParametersDialog(std::map<QString, Argument> *argsMap, QWidget *parent_ = 0);

    private slots:
        void okClicked();
        void resetClicked();

    private:
        std::map<QString, Argument> *m_argsMap;
        std::map<QString, QWidget *> *m_widgetsMap; // contains editable widgets
};

#endif
