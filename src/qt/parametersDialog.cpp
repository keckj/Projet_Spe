#include <cmath>
#include "headers.hpp"
#include "parametersDialog.hpp"
#include "parametersDialog.moc"
#include "sidePanel.hpp"
#include "argument.hpp"
#include "value.hpp"


ParametersDialog::ParametersDialog(std::map<std::string, Argument> *argsMap, QWidget *parent_) : QDialog(parent_) {

    // Get parent
    SidePanel *panel = qobject_cast<SidePanel *>(parent_);
    if (!panel)
        log4cpp::log_console->errorStream() << "ParametersDialog does not have a parent !";

    this->setWindowTitle("Parameters");

    // Layouts
    QGridLayout *mainGridLayout = new QGridLayout;
    QGridLayout *argGridLayout = new QGridLayout;
    QHBoxLayout *buttonsLayout = new QHBoxLayout;

    this->setLayout(mainGridLayout);
    mainGridLayout->addLayout(argGridLayout, 0, 0);
    mainGridLayout->addLayout(buttonsLayout, 1, 0);

    // OK / Reset / Cancel buttons
    QPushButton *okButton = new QPushButton("OK");
    buttonsLayout->addWidget(okButton);
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));

    QPushButton *resetButton = new QPushButton("Reset");
    buttonsLayout->addWidget(resetButton);
    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetClicked()));

    QPushButton *cancelButton = new QPushButton("Cancel");
    buttonsLayout->addWidget(cancelButton);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    // Arguments
    m_argsMap = argsMap;
    m_widgetsMap = new std::map<std::string, QWidget *>; 
    const int nbParamsPerColumn = 10;
    int cpt = 0;
    QWidget *widget;

    for (auto it = m_argsMap->begin(); it != m_argsMap->end(); ++it) {
        argGridLayout->addWidget(new QLabel(QString(it->first.c_str())), cpt % nbParamsPerColumn, 2 * (cpt / nbParamsPerColumn));
        Argument arg = it->second;
        switch(arg.preferredWidgetType()) {
            case SLIDER:    // always int
                if (arg.type() != INT) {
                    log4cpp::log_console->errorStream() <<  "Type " << arg.type() << " is not supported in QSlider !";
                } else {
                    widget = new QSlider(Qt::Horizontal);
                    ((QSlider *)widget)->setRange(arg.minVal(), arg.maxVal());
                    ((QSlider *)widget)->setValue(arg.val());
                }
                break;
            case SPINBOX:   // int or double
                switch (arg.type()) {
                    case INT:
                        widget = new QSpinBox;
                        ((QSpinBox *)widget)->setMinimum(arg.minVal());
                        ((QSpinBox *)widget)->setMaximum(arg.maxVal());
                        ((QSpinBox *)widget)->setValue(arg.val());
                        break;
                    case FLOAT:
                        widget = new QDoubleSpinBox;
                        ((QDoubleSpinBox *)widget)->setDecimals((int)(-log10((float)arg.minVal()))+2);
                        ((QDoubleSpinBox *)widget)->setSingleStep(((float)arg.maxVal()-(float)arg.minVal())/100.0);
                        ((QDoubleSpinBox *)widget)->setMinimum((float)arg.minVal());
                        ((QDoubleSpinBox *)widget)->setMaximum((float)arg.maxVal());
                        ((QDoubleSpinBox *)widget)->setValue((float)arg.val());
						break;
                    case DOUBLE:
                        widget = new QDoubleSpinBox;
                        ((QDoubleSpinBox *)widget)->setDecimals((int)(-log10((double)arg.minVal()))+2);
                        ((QDoubleSpinBox *)widget)->setSingleStep(((double)arg.maxVal()-(double)arg.minVal())/100.0);
                        ((QDoubleSpinBox *)widget)->setMinimum((double)arg.minVal());
                        ((QDoubleSpinBox *)widget)->setMaximum((double)arg.maxVal());
                        ((QDoubleSpinBox *)widget)->setValue((double)arg.val());
                        break;
                    default:
                        log4cpp::log_console->errorStream() << "Type " << arg.type() << " is not supported in QSpinBox / QDoubleSpinBox !";
                }
                break;
            case CHECKBOX:  // always bool
                if (arg.type() != BOOL) {
                        log4cpp::log_console->errorStream() << "Type " << arg.type() << " is not supported in QCheckBox !";
                } else {
                    widget = new QCheckBox;
                    ((QCheckBox *)widget)->setChecked(arg.val());
                }
                break;
            default:
                log4cpp::log_console->errorStream() << "ParametersDialog: Unknown argument widget type !";
                break;
        }
        argGridLayout->addWidget(widget, cpt % nbParamsPerColumn, (2 * (cpt / nbParamsPerColumn)) + 1);
        m_widgetsMap->emplace(it->first, widget);
        cpt++;
    }
}

void ParametersDialog::okClicked() {
    for (auto it = m_widgetsMap->begin(); it != m_widgetsMap->end(); ++it) {
        QWidget *widget = it->second;
        Argument arg = m_argsMap->at(it->first);
        switch(arg.preferredWidgetType()) {
            case SLIDER:    // always int
                arg() = ((QSlider *)widget)->value();
                break;
            case SPINBOX:   // int or double/float
                if (arg.type() == INT) {
                    arg() = ((QSpinBox *)widget)->value();
                } else {
                    arg() = ((QDoubleSpinBox *)widget)->value();
                }
                break;
            case CHECKBOX:  // always bool
                arg() = ((QCheckBox *)widget)->isChecked();
                break;
        }
    }

    emit accept(); 
}

void ParametersDialog::resetClicked() {
    for (auto it = m_widgetsMap->begin(); it != m_widgetsMap->end(); ++it) {
        QWidget *widget = it->second;
        Argument arg = m_argsMap->at(it->first);
        switch(arg.preferredWidgetType()) {
            case SLIDER:    // always int
                ((QSlider *)widget)->setValue(arg.defaultVal());
                break;
            case SPINBOX:   // int or double/float
                if (arg.type() == INT) {
                    ((QSpinBox *)widget)->setValue(arg.defaultVal());
                } else {
                    ((QDoubleSpinBox *)widget)->setValue(arg.defaultVal());
                }
                break;
            case CHECKBOX:  // always bool
                ((QCheckBox *)widget)->setChecked(arg.defaultVal());
                break;
        }
    }
}

