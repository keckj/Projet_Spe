#include "headers.hpp"
#include "sidePanel.hpp"
#include "sidePanel.moc"
#include "mainWindow.hpp"
#include "parametersDialog.hpp"
#include "argument.hpp"
#include "colormap.hpp"

SidePanel::SidePanel(QWidget *parent_) : QWidget(parent_) {

    // Get parent
    MainWindow *mainWin = qobject_cast<MainWindow *>(parent_);
    if (!mainWin)
        log4cpp::log_console->errorStream() << "SidePanel does not have a parent !";

    m_paused = true;

    this->setStyleSheet("QWidget {background-color: white;}");
    this->setAutoFillBackground(true);

    // GroupBox
    QGroupBox *modelGroupBox = new QGroupBox("Model options");
    QGroupBox *runGroupBox = new QGroupBox("Runtime options");
    QGroupBox *renderOptionsGroupBox = new QGroupBox("Rendering options");

    modelGroupBox->setStyleSheet(
										"QGroupBox {"
										"    border: 1px solid gray;"
										"    border-radius: 9px;"
										"    margin-top: 0.5em;"
										"}"
										""
										"QGroupBox::title {"
										"    subcontrol-origin: margin;"
										"    left: 20px;"
										"    padding: 0 3px 0 3px;"
										"}");
    runGroupBox->setStyleSheet(
										"QGroupBox {"
										"    border: 1px solid gray;"
										"    border-radius: 9px;"
										"    margin-top: 0.5em;"
										"}"
										""
										"QGroupBox::title {"
										"    subcontrol-origin: margin;"
										"    left: 20px;"
										"    padding: 0 3px 0 3px;"
										"}");
    renderOptionsGroupBox->setStyleSheet(
										"QGroupBox {"
										"    border: 1px solid gray;"
										"    border-radius: 9px;"
										"    margin-top: 0.5em;"
										"}"
										""
										"QGroupBox::title {"
										"    subcontrol-origin: margin;"
										"    left: 20px;"
										"    padding: 0 3px 0 3px;"
										"}");

    // Labels for model
    QLabel *modelLabel = new QLabel("Selected model :");
    QLabel *iterLabel = new QLabel("Iterations : ");
    
    // Dropdown list
    modelComboBox = new QComboBox();
    modelComboBox->addItem("Default model");
    modelComboBox->addItem("Simple Model 2D");
    connect(modelComboBox, SIGNAL(currentIndexChanged(int)), mainWin, SLOT(changeModel(int)));

    // Iterations spinBox
    iterSpinBox = new QSpinBox();
    iterSpinBox->setRange(1, 1000000);
    iterSpinBox->setSingleStep(10);
    iterSpinBox->setValue(100);
    connect(iterSpinBox, SIGNAL(valueChanged(int)), mainWin, SLOT(changeNbIter(int)));
    connect(iterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeNbIterSlider(int)));

    // Button for model parameters
    paramsButton = new QPushButton("Parameters");
    connect(paramsButton, SIGNAL(clicked()), this, SLOT(openParametersDialog()));
    paramsDialog = new ParametersDialog(this);

    // Button for QFileDialog
    saveDirButton = new QPushButton("Choose saving directory");
    connect(saveDirButton, SIGNAL(clicked()), this, SLOT(changeDirectory()));

    // Buttons
    startButton = new QPushButton("Start");
    connect(startButton, SIGNAL(clicked()), this, SLOT(start_pause_resume()));
    connect(this, SIGNAL(startPushed()), mainWin, SLOT(startComputing()));
    connect(this, SIGNAL(pauseOrResumePushed(bool)), mainWin, SLOT(pauseComputing(bool)));
    stopButton = new QPushButton("Stop");
    stopButton->setEnabled(false);
    connect(stopButton, SIGNAL(clicked()), mainWin, SLOT(stopComputing()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));

    // Rendering colormap label & dropdown list
    QLabel *colorLabel = new QLabel("<FONT COLOR='red'>TEST</FONT>");
    QComboBox *colorComboBox = new QComboBox();
    std::map<std::string, std::pair<unsigned int, float*>> colormap = ColorMap::multiHueColorMaps();
    for (auto it = colormap.begin(); it != colormap.end(); ++it) {
        colorComboBox->addItem(QString(it->first.c_str()));
    }
    connect(colorComboBox, SIGNAL(currentIndexChanged(const QString &)), mainWin, SLOT(changeColormap(const QString &)));

    // Auto rendering checkbox
    QCheckBox *autoRenderCheckBox = new QCheckBox("Automatic rendering");
    autoRenderCheckBox->setChecked(true);
    connect(autoRenderCheckBox, SIGNAL(stateChanged(int)), mainWin, SLOT(changeAutoRendering(int)));
    connect(autoRenderCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showSlider(int)));
    
    // Iteration selection slider
    gridSlider = new QSlider(Qt::Horizontal);
    gridSlider->setRange(1, 100); // to be updated when iterSpinBox changes value
    gridSlider->setPageStep(5);
    gridSlider->setSingleStep(1);
    gridSlider->setTracking(false);
    gridSlider->setValue(1);
    gridSlider->hide();
    connect(gridSlider, SIGNAL(valueChanged(int)), mainWin, SLOT(changeDisplayedGrid(int)));
    
    // Layouts
    QBoxLayout *globalLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    QBoxLayout *modelLayout = new QBoxLayout(QBoxLayout::TopToBottom, modelGroupBox);
    QBoxLayout *runLayout = new QBoxLayout(QBoxLayout::TopToBottom, runGroupBox);
    QBoxLayout *renderOptionsLayout = new QBoxLayout(QBoxLayout::TopToBottom, renderOptionsGroupBox);

    this->setLayout(globalLayout);
    modelGroupBox->setLayout(modelLayout);
    runGroupBox->setLayout(runLayout);
    renderOptionsGroupBox->setLayout(renderOptionsLayout);

    // Add Widgets
    globalLayout->setSpacing(30);
    globalLayout->addWidget(modelGroupBox);
    globalLayout->addWidget(runGroupBox);
    globalLayout->addWidget(renderOptionsGroupBox);

    modelLayout->setSpacing(10);
    modelLayout->addWidget(modelLabel);
    modelLayout->addWidget(modelComboBox);
    modelLayout->addWidget(iterLabel);
    modelLayout->addWidget(iterSpinBox);
    modelLayout->addWidget(paramsButton);

    runLayout->setSpacing(10);
    runLayout->addWidget(startButton);
    runLayout->addWidget(stopButton);
    runLayout->addWidget(saveDirButton);

    renderOptionsLayout->setSpacing(10);
    renderOptionsLayout->addWidget(colorLabel);
    renderOptionsLayout->addWidget(colorComboBox);
    renderOptionsLayout->addWidget(autoRenderCheckBox);
    renderOptionsLayout->addWidget(gridSlider);
}

SidePanel::~SidePanel() {
}

void SidePanel::start_pause_resume() {
    setModelOptionsStatus(false);
    stopButton->setEnabled(true);
    m_paused = !m_paused;
    if (m_paused) {
        emit pauseOrResumePushed(true);
        startButton->setText("Resume");
    } else {
        if (startButton->text() != "Start")
            emit pauseOrResumePushed(false);
        else 
            emit startPushed();
        startButton->setText("Pause");
    }
}

void SidePanel::stop() {
    setModelOptionsStatus(true);
    stopButton->setEnabled(false);
    startButton->setText("Start");
    m_paused = true;
}

void SidePanel::setModelOptionsStatus(bool status) {
    this->modelComboBox->setEnabled(status);
    this->iterSpinBox->setEnabled(status);
    this->paramsButton->setEnabled(status);
    this->saveDirButton->setEnabled(status);
}

void SidePanel::openParametersDialog() {
    paramsDialog->setModal(true);
    paramsDialog->show();    
}
        
std::map<std::string, Argument> *getArguments() {
    //switch(modelComboBox)
    return new std::map<std::string, Argument>;
}

void SidePanel::changeDirectory() {
    QFileDialog dialog;
    dialog.setDirectory(QDir::currentPath());
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (dialog.exec()) {
        m_saveDirectory = dialog.selectedFiles().at(0);
    }
}

void SidePanel::showSlider(int checkboxState) {
    if (checkboxState == Qt::Unchecked)
        this->gridSlider->show();
    else
        this->gridSlider->hide();    
}

void SidePanel::changeNbIterSlider(int nbIter) {
    this->gridSlider->setRange(1, nbIter);
}

void SidePanel::keyPressEvent(QKeyEvent *k) {
    switch (k->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            start_pause_resume();
            break;
    }
    emit childKeyEvent(k);
}

