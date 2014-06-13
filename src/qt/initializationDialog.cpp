#include "headers.hpp"
#include "initializationDialog.hpp"
#include "initializationDialog.moc"
#include "sidePanel.hpp"


InitializationDialog::InitializationDialog(unsigned int *gridWidth, unsigned int *gridHeight, unsigned int *gridLength, QWidget *parent_) :
    QDialog(parent_) {

    // Get parent
    SidePanel *panel = qobject_cast<SidePanel *>(parent_);
    if (!panel)
        log4cpp::log_console->errorStream() << "InitializationDialog does not have a parent !";

    this->setWindowTitle("Initialization");

    // Copy pointers
    m_gridWidth = gridWidth;
    m_gridHeight = gridHeight;
    m_gridLength = gridLength;

    // Layouts
    QGridLayout *mainGridLayout = new QGridLayout;
    QGridLayout *sizeGridLayout = new QGridLayout;
    QGridLayout *initGridLayout = new QGridLayout;
    QHBoxLayout *buttonsLayout = new QHBoxLayout;

    this->setLayout(mainGridLayout);
    mainGridLayout->addLayout(sizeGridLayout, 0, 0);
    mainGridLayout->addLayout(buttonsLayout, 1, 0);

    // 3D grid checkbox
    grid3DCheckbox = new QCheckBox("Use 3D grid");
    grid3DCheckbox->setChecked(*gridLength > 1);
    sizeGridLayout->addWidget(grid3DCheckbox, 0, 0);
    connect(grid3DCheckbox, SIGNAL(stateChanged(int)), this, SLOT(changeGridDimension(int)));

    // Grid size
    QLabel *gridSizeLabel = new QLabel("Grid size :");
    sizeGridLayout->addWidget(gridSizeLabel, 1, 0); 

    widthSpinBox = new QSpinBox;
    widthSpinBox->setMinimum(1);
    widthSpinBox->setMaximum(8192);
    widthSpinBox->setValue(*gridWidth);
    sizeGridLayout->addWidget(widthSpinBox, 1, 1);

    heightSpinBox = new QSpinBox;
    heightSpinBox->setMinimum(1);
    heightSpinBox->setMaximum(8192);
    heightSpinBox->setValue(*gridHeight);
    sizeGridLayout->addWidget(heightSpinBox, 1, 2);
    
    lengthSpinBox = new QSpinBox;
    lengthSpinBox->setMinimum(1);
    lengthSpinBox->setMaximum(8192);
    lengthSpinBox->setValue(*gridLength);
    lengthSpinBox->setVisible(*gridLength > 1);
    sizeGridLayout->addWidget(lengthSpinBox, 1, 3);

    // Grid init TODO
    

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


} 
      
InitializationDialog::~InitializationDialog() {
}

void InitializationDialog::okClicked() {
    *m_gridWidth = widthSpinBox->value();
    *m_gridHeight = heightSpinBox->value();
    *m_gridLength = lengthSpinBox->value();
    emit accept();
}

void InitializationDialog::resetClicked() {
    grid3DCheckbox->setChecked(SidePanel::defaultGridSize[2] > 1);
    lengthSpinBox->setVisible(SidePanel::defaultGridSize[2] > 1);
    widthSpinBox->setValue(SidePanel::defaultGridSize[0]);
    heightSpinBox->setValue(SidePanel::defaultGridSize[1]);
    lengthSpinBox->setValue(SidePanel::defaultGridSize[2]);
}

void InitializationDialog::changeGridDimension(int checkboxState) {
    if (checkboxState == Qt::Unchecked) {
        lengthSpinBox->hide();
        lengthSpinBox->setValue(1);
    } else {
        lengthSpinBox->show();
        lengthSpinBox->setValue(512);
    }
}

