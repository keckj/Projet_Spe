#include "headers.hpp"
#include "parametersDialog.hpp"
#include "parametersDialog.moc"
#include "sidePanel.hpp"
#include "argument.hpp"
#include "value.hpp"


ParametersDialog::ParametersDialog(QWidget *parent_) : QDialog(parent_) {

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

    m_argsMap = panel->getArguments();
    
    int nParams = m_argsMap->size();
    const int nbParamsPerColumn = 10;
    int cpt = 0;
    QWidget *widget;
    
    for (auto it = m_argsMap->begin(); it != m_argsMap->end(); ++it) {
        argGridLayout->addWidget(new QLabel(QString(it->first.c_str())), cpt % nbParamsPerColumn, 2 * cpt / nbParamsPerColumn);
        cpt++;
        Argument arg = it->second;
        switch(arg.preferredWidgetType()) {
            case SLIDER:    // always int
                widget = new QSlider;
                ((QSlider*)widget)->setValue(arg.val());
                ((QSlider*)widget)->setRange(arg.minVal(), arg.maxVal());
                break;
            case SPINBOX:   // int or double
                if (arg.type() == INT) {
                    widget = new QSpinBox;
                    ((QSpinBox *)widget)->setValue(arg.val());
                    ((QSpinBox *)widget)->setMaximum(arg.maxVal());
                    ((QSpinBox *)widget)->setMinimum(arg.minVal());
                } else {
                    widget = new QDoubleSpinBox;
                    ((QDoubleSpinBox*)widget)->setValue(arg.val());
                    ((QDoubleSpinBox*)widget)->setMaximum(arg.maxVal());
                    ((QDoubleSpinBox*)widget)->setMinimum(arg.minVal());
                }
                break;
            case CHECKBOX:  // always bool
                widget = new QCheckBox;
                ((QCheckBox*)widget)->setChecked(arg.val());
                break;
            default:
                log4cpp::log_console->errorStream() << "ParametersDialog: Unknown argument widget type !";
                break;
        }
        argGridLayout->addWidget(widget, cpt % nbParamsPerColumn, (2 * cpt / nbParamsPerColumn) + 1);
        cpt++;
    }
}

void ParametersDialog::okClicked() {
    // TODO call method in SidePanel
    emit accept(); 
}

void ParametersDialog::resetClicked() {
    // TODO reset all widget->setValue(arg->defaultVal())
}

