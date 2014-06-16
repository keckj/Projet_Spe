
#include "headers.hpp"
#include "codeEditorGui.hpp"
#include "codeEditorGui.moc"
#include "pythonCodeInitialCond.hpp"

CodeEditorGui::CodeEditorGui(QWidget *parent_) : 
	QWidget(parent_),
	_actualCodeType(PYTHON_FUNC),
	_defaultPythonFunc("return x*y*z"),
	_defaultPythonCode("import math\n\ndef F%1(x,y,z):\n\treturn x*math.cos(y)*z\n")
{

	_layout = new QVBoxLayout();
	_label = new QLabel("Enter python code:");
	_codeEditor = new CodeEditor();
	_submitButton = new QPushButton("Submit");

	_layout->addWidget(_label);
	_layout->addWidget(_codeEditor);
	_layout->addWidget(_submitButton);
	
	this->setLayout(_layout);
	this->setWindowTitle("Python code editor");

	connect(_submitButton, SIGNAL(clicked()), this, SLOT(submitCode()));
}
     
CodeEditorGui::~CodeEditorGui() {
}

void CodeEditorGui::showCodeEditor(int pythonCodeType) {
	switch(pythonCodeType) {
		case PYTHON_FUNC:
			_label->setText("Enter return value (input = x,y,z):");
			_codeEditor->clear();
			_codeEditor->insertPlainText(_defaultPythonFunc);
			break;
		case PYTHON_CODE:
			{
				_label->setText(
						QString("Enter python code (the function that will be called is F%1):").arg(PythonCodeInitialCond<float>::getCurrentId()));
				_codeEditor->clear();
				_codeEditor->insertPlainText(_defaultPythonCode.arg(PythonCodeInitialCond<float>::getCurrentId()));
				break;
			}
		default:
			log4cpp::log_console->infoStream() << "Got unknown python code type, aborting !";
			exit(EXIT_FAILURE);
	}

	_actualCodeType = static_cast<PythonCodeType>(pythonCodeType);

	show();
}

void CodeEditorGui::submitCode() {
	const QString code(_codeEditor->toPlainText());
	hide();
	emit codeSubmitted(code);
}
