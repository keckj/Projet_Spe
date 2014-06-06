#include "headers.hpp"
#include "initializationDialog.hpp"
#include "initializationDialog.moc"
#include "sidePanel.hpp"


InitializationDialog::InitializationDialog(QWidget *parent_) : QDialog(parent_) {

    // Get parent
    SidePanel *panel = qobject_cast<SidePanel *>(parent_);
    if (!panel)
        log4cpp::log_console->errorStream() << "InitializationDialog does not have a parent !";

    this->setWindowTitle("Initialization");

    // Layouts
    QGridLayout *mainGridLayout = new QGridLayout;
    QGridLayout *sizeGridLayout = new QGridLayout;
    //TODO initGridLayout
    QHBoxLayout *buttonsLayout = new QHBoxLayout;

    this->setLayout(mainGridLayout);
    mainGridLayout->addLayout(sizeGridLayout, 0, 0);
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

    // Grid size
    QLabel *gridSizeLabel = new QLabel("Grid size :");
    sizeGridLayout->addWidget(gridSizeLabel, 0, 0); 

    widthSpinBox = new QSpinBox;
    widthSpinBox->setMinimum(1);
    widthSpinBox->setMaximum(8192);
    widthSpinBox->setValue(512);
    sizeGridLayout->addWidget(widthSpinBox, 0, 1);

    heightSpinBox = new QSpinBox;
    heightSpinBox->setMinimum(1);
    heightSpinBox->setMaximum(8192);
    heightSpinBox->setValue(512);
    sizeGridLayout->addWidget(heightSpinBox, 0, 2);
    
    lengthSpinBox = new QSpinBox;
    lengthSpinBox->setMinimum(1);
    lengthSpinBox->setMaximum(8192);
    lengthSpinBox->setValue(1);
    sizeGridLayout->addWidget(lengthSpinBox, 0, 3);

    // Grid init TODO
} 
      
void InitializationDialog::okClicked() {
    emit gridSizeUpdate(widthSpinBox->value(), heightSpinBox->value(), lengthSpinBox->value()); 
    emit accept();
}

void InitializationDialog::resetClicked() {
    widthSpinBox->setValue(512);
    heightSpinBox->setValue(512);
    lengthSpinBox->setValue(1);
}

