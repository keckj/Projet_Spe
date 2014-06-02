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

    //vector<Argument *> *args = panel->getArguments();

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

    /*
    for (auto arg = args->begin(); arg != args->end(); ++arg) {
        argGridLayout->addWidget(new QLabel(arg->name()), TODO, TODO); 
        switch(arg->preferredWidgetType()) {
            case SLIDER:    // always int
                widget = new QSlider;
                widget->setValue(arg->val().int());
                widget->setRange(arg->minVal(), arg->maxVal());
                break;
            case SPINBOX:   // int or double
                if (arg->type() == INT) {
                    widget = new QSpinBox;
                    widget->setValue(arg->val().int());
                    widget->setMaximum(arg->maxVal());
                    widget->setMinimum(arg->minVal());
                } else {
                    widget = new QDoubleSpinBox;
                    widget->setValue(arg->val().double());
                    widget->setMaximum(arg->maxVal());
                    widget->setMinimum(arg->minVal());
                }
                break;
            case CHECKBOX:  // always bool
                widget = new QCheckBox;
                widget->setChecked(arg->val().bool());
                break;
            default:
                log4cpp::log_console->errorStream() << "ParametersDialog: Unknown argument widget type !";
                break;
        }
        argGridLayout->addWidget(widget, TODO, TODO);
    }*/
}

void ParametersDialog::okClicked() {
    // TODO call method in SidePanel
    emit accept(); 
}

void ParametersDialog::resetClicked() {
    // TODO reset all widget->setValue
}

