
#include "headers.hpp"
#include "codeEditor.hpp"

#ifndef CODEEDITORGUI_H
#define CODEEDITORGUI_H

enum PythonCodeType {
	PYTHON_FUNC=0,
	PYTHON_CODE
};

class CodeEditorGui : public QWidget
{
	Q_OBJECT
	
	public:
		CodeEditorGui(QWidget *parent=0);
		~CodeEditorGui();

	private:
		QVBoxLayout *_layout;
		CodeEditor *_codeEditor;
		QPushButton *_submitButton;
		QLabel *_label;

		PythonCodeType _actualCodeType;
		const QString _defaultPythonFunc;
		const QString _defaultPythonCode;

 
	public slots:
		void showCodeEditor(int pythonCodeType);

	private slots:
		void submitCode();

	signals:
		void codeSubmitted(const QString &);


};

#endif /* end of include guard: CODEEDITORGUI_H */

