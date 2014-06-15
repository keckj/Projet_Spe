#include "headers.hpp"
#include "sidePanel.hpp"
#include "sidePanel.moc"
#include "mainWindow.hpp"
#include "parametersDialog.hpp"
#include "initializationDialog.hpp"
#include "argument.hpp"
#include "initialCondFactory.hpp"
#include "colormap.hpp"

// Models
#include "simpleModel2D.hpp"
#include "multigpu.hpp"



// ************************
const QStringList SidePanel::modelsList = QStringList() << "Simple Model 2D" << "->Multi-GPU<-" << "Default Model";
const int SidePanel::defaultNumberOfSteps = 10000;
const std::vector<unsigned int> SidePanel::defaultGridSize {512, 512, 1};
const int SidePanel::defaultNbStepsToSave = 100;
// ************************



SidePanel::SidePanel(QWidget *parent_) : 
    QWidget(parent_),
	m_paused(true),
    m_gridWidth(defaultGridSize[0]),
    m_gridHeight(defaultGridSize[1]),
    m_gridLength(defaultGridSize[2]),
    m_argsMap(0),
    m_varsMap(0),
    m_initialCondsMap(0),
	m_defaultCondsMap(0),
    m_saveDirectory(""),
    initDialog(0),
    paramsDialog(0)
{

    // Get parent
    MainWindow *mainWin = qobject_cast<MainWindow *>(parent_);
    if (!mainWin)
        log4cpp::log_console->errorStream() << "SidePanel does not have a parent !";

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
    
    // Layouts
    QBoxLayout *globalLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    QGridLayout *modelLayout = new QGridLayout(modelGroupBox);
    QGridLayout *runLayout = new QGridLayout(runGroupBox);
    QGridLayout *renderOptionsLayout = new QGridLayout(renderOptionsGroupBox);

    this->setLayout(globalLayout);
    modelGroupBox->setLayout(modelLayout);
    runGroupBox->setLayout(runLayout);
    renderOptionsGroupBox->setLayout(renderOptionsLayout);

    //----//

    // Labels for model
    QLabel *modelLabel = new QLabel("Selected model :");
    QLabel *iterLabel = new QLabel("Iterations :");
    
    // Dropdown list
    modelComboBox = new QComboBox;
    modelComboBox->addItems(SidePanel::modelsList);
    connect(modelComboBox, SIGNAL(currentIndexChanged(int)), mainWin, SLOT(changeModel(int)));
    connect(modelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshParameters(int)));

    // Iterations spinBox
    iterSpinBox = new QSpinBox();
    iterSpinBox->setRange(1, 1000000);
    iterSpinBox->setSingleStep(defaultNumberOfSteps/10);
    iterSpinBox->setValue(defaultNumberOfSteps);
    connect(iterSpinBox, SIGNAL(valueChanged(int)), mainWin, SLOT(changeNbIter(int)));
    //connect(iterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeNbIterSlider(int)));

    // Buttons for model parameters and model initialization with QDialog
    initButton = new QPushButton("Initialization");
    connect(initButton, SIGNAL(clicked()), this, SLOT(openInitDialog()));
    paramsButton = new QPushButton("Parameters");
    connect(paramsButton, SIGNAL(clicked()), this, SLOT(openParametersDialog()));
    
    //---

    // Buttons
    startButton = new QPushButton("Start");
    connect(startButton, SIGNAL(clicked()), this, SLOT(start_pause_resume()));
    connect(this, SIGNAL(startPushed()), mainWin, SLOT(startComputing()));
    connect(this, SIGNAL(pauseOrResumePushed(bool)), mainWin, SLOT(pauseComputing(bool)));
    stopButton = new QPushButton("Stop");
    stopButton->setEnabled(false);
    connect(stopButton, SIGNAL(clicked()), mainWin, SLOT(stopComputing()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));

    // Checkbox for saving option
    saveDirCheckBox = new QCheckBox("Save data");
    saveDirCheckBox->setChecked(false);

    // Button for QFileDialog
    saveDirButton = new QPushButton("Choose saving directory");
    connect(saveDirButton, SIGNAL(clicked()), this, SLOT(changeDirectory()));

    // Steps to skip Label
    QLabel *saveStepsLabel = new QLabel("Steps between saves :");

    // Steps to skip Spinbox
    saveStepsSpinBox = new QSpinBox();
    saveStepsSpinBox->setRange(1, 1000000);
    saveStepsSpinBox->setSingleStep(1);
    saveStepsSpinBox->setValue(defaultNbStepsToSave);
    connect(iterSpinBox, SIGNAL(valueChanged(int)), mainWin, SLOT(changeNbStepsToSave(int)));

    //---

    // Labels and lists for variables to display
    QLabel *variablesRenderedLabel = new QLabel("Variables rendered :");
    variablesRenderedList = new QListWidget;
    variablesRenderedList->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
    variablesRenderedList->setMinimumSize(QSize(30,100)); //modify if needed (height = nItems * cst?)
    connect(variablesRenderedList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(updateRenderedVars(QListWidgetItem *)));

    // Rendering colormap label & dropdown list
    QLabel *colorLabel = new QLabel("Colormap :");
    QComboBox *colorComboBox = new QComboBox;
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
    autoRenderCheckBox->setVisible(false);
    
    // Iteration selection slider
    gridSlider = new QSlider(Qt::Horizontal);
    gridSlider->setRange(1, 100); // to be updated when iterSpinBox changes value
    gridSlider->setPageStep(5);
    gridSlider->setSingleStep(1);
    gridSlider->setTracking(false);
    gridSlider->setValue(1);
    gridSlider->hide();
    connect(gridSlider, SIGNAL(valueChanged(int)), mainWin, SLOT(changeDisplayedGrid(int)));
    
    //----//

    // Add Widgets
    globalLayout->setSpacing(30);
    globalLayout->addWidget(modelGroupBox);
    globalLayout->addWidget(runGroupBox);
    globalLayout->addWidget(renderOptionsGroupBox);

    modelLayout->setSpacing(10);
    modelLayout->addWidget(modelLabel, 0, 0);
    modelLayout->addWidget(modelComboBox, 0, 1);
    modelLayout->addWidget(iterLabel, 1, 0);
    modelLayout->addWidget(iterSpinBox, 1, 1);
    modelLayout->addWidget(initButton, 2, 0);
    modelLayout->addWidget(paramsButton, 2, 1);

    runLayout->setSpacing(10);
    runLayout->addWidget(startButton, 0, 0, 1, 2);
    runLayout->addWidget(stopButton, 1, 0, 1, 2);
    runLayout->addWidget(saveDirCheckBox, 2, 0);
    runLayout->addWidget(saveDirButton, 2, 1);
    runLayout->addWidget(saveStepsLabel, 3, 0);
    runLayout->addWidget(saveStepsSpinBox, 3, 1);

    renderOptionsLayout->setSpacing(10);
    renderOptionsLayout->addWidget(variablesRenderedLabel, 0, 0);
    renderOptionsLayout->addWidget(variablesRenderedList, 0, 1);
    renderOptionsLayout->addWidget(colorLabel, 1, 0);
    renderOptionsLayout->addWidget(colorComboBox,1 , 1);
    renderOptionsLayout->addWidget(autoRenderCheckBox, 2, 0);
    renderOptionsLayout->addWidget(gridSlider, 2, 1);
    
    // Init m_argsMap and m_varsMap
    refreshParameters(modelComboBox->currentIndex());
}

SidePanel::~SidePanel() {
}

void SidePanel::start_pause_resume() {
    // Check that the saving directory was set if asked to save data
    if (saveDirCheckBox->isChecked() && m_saveDirectory == "") {
        const QString message = "Please choose a saving directory before starting the simulation.";
        QMessageBox msgBox(QMessageBox::Warning, "No saving directory chosen !", message, 0, this);
        msgBox.addButton("Choose saving directory", QMessageBox::AcceptRole);
        msgBox.addButton("Cancel", QMessageBox::RejectRole);
        if (msgBox.exec() == QMessageBox::AcceptRole) {
            changeDirectory();
        }
        return;
    }

    setModelOptionsStatus(false);
    stopButton->setEnabled(true);
    m_paused = !m_paused;
    if (m_paused) {
        emit pauseOrResumePushed(true);
        startButton->setText("Resume");
    } else {
        if (startButton->text() != "Start") {
            emit pauseOrResumePushed(false);
        } else {
            emit startPushed();
        }
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
    this->initButton->setEnabled(status);
    this->paramsButton->setEnabled(status);
    this->saveDirCheckBox->setEnabled(status);
    this->saveDirButton->setEnabled(status);
    this->saveStepsSpinBox->setEnabled(status);
}

void SidePanel::openInitDialog() {
    if (initDialog) initDialog->deleteLater();

    initDialog = new InitializationDialog(m_varsMap, m_initialCondsMap, m_defaultCondsMap, &m_gridWidth, &m_gridHeight, &m_gridLength, this);
    initDialog->setModal(true);
    initDialog->show();
}

void SidePanel::openParametersDialog() {
    if (paramsDialog) paramsDialog->deleteLater();

    paramsDialog = new ParametersDialog(m_argsMap, this);
    paramsDialog->setModal(true);
    paramsDialog->show();
}

void SidePanel::changeDirectory() {
    QFileDialog dialog(this, "Choose simulation data saving directory");
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

void SidePanel::refreshParameters(int modelId) {

	delete m_argsMap;
	delete m_varsMap;
	delete m_defaultCondsMap;

    switch(modelId) {
        case 0:
            m_argsMap = SimpleModel2D::getArguments();
            m_varsMap = SimpleModel2D::getVariables();
            break;
        case 1:
            m_argsMap = MultiGpu::getArguments();
            m_varsMap = MultiGpu::getVariables();
            m_defaultCondsMap = MultiGpu::getDefaultInit();
            break;
        default:
            m_argsMap = new std::map<QString, Argument>;
            m_varsMap = new std::map<QString, bool>;
    }
    
    if (m_initialCondsMap) {
        m_initialCondsMap->clear();
    } else {
        m_initialCondsMap = new std::map<QString, int>;
    }

    for (auto it = m_varsMap->begin(); it != m_varsMap->end(); ++it) {
		if(m_defaultCondsMap && m_defaultCondsMap->find(it->first) != m_defaultCondsMap->end())
			m_initialCondsMap->emplace(it->first, m_defaultCondsMap->at(it->first));
		else
			m_initialCondsMap->emplace(it->first, 0);
    }

    // Variables: GUI update
    this->variablesRenderedList->clear();    
    QListWidgetItem *item;
    for (auto it = m_varsMap->begin(); it !=m_varsMap->end(); ++it) {
        item = new QListWidgetItem(it->first);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        if (it->second) {
            item->setCheckState(Qt::Checked);
        } else {
            item->setCheckState(Qt::Unchecked);
        }
        this->variablesRenderedList->addItem(item);
    }
}

void SidePanel::updateRenderedVars(QListWidgetItem *item) {
    m_varsMap->at(item->text()) = (item->checkState() != Qt::Unchecked);
}

unsigned int SidePanel::getGridWidth() {
    return m_gridWidth;
}

unsigned int SidePanel::getGridHeight() {
    return m_gridHeight;
}

unsigned int SidePanel::getGridLength() {
    return m_gridLength;
}

std::map<QString, Argument> *SidePanel::getArguments() {
    return m_argsMap;
}

std::map<QString, bool> *SidePanel::getVariables() {
    return m_varsMap;
}

std::map<QString, int> *SidePanel::getInitialConditions() {
    return m_initialCondsMap;
}

QString SidePanel::getSaveDirectory() {
    if (saveDirCheckBox->isChecked()) {
        return m_saveDirectory;
    } else {
        return "";
    }
}

void SidePanel::keyPressEvent(QKeyEvent *k) {
    emit childKeyEvent(k);
}

