#include "headers.hpp"

#include "initialCondFactory.hpp"
#include "initialCondFactory.moc"

#include "initialCond.hpp"
#include "circleInitialCond.hpp"
#include "functionInitialCond.hpp"
#include "pythonFuncInitialCond.hpp"
#include "pythonCodeInitialCond.hpp"
#include "codeEditorGui.hpp"

#include <thread>

using namespace log4cpp;

InitialCond<float> *InitialCondFactory::getInitialCond(int i) {

	log_console->infoStream() << "Creating a '" << getInitialCondName(i).toUtf8().constData() << "' initial condition...";

	switch (i) {
		case 0:
			return new FunctionInitialCond<float>([] (float,float,float)->float {return 0;});
		case 1:
			return new FunctionInitialCond<float>([] (float,float,float)->float {return 1/2.0;});
		case 2:
			return new FunctionInitialCond<float>([] (float,float,float)->float {return 1;});
		case 3:
			return new FunctionInitialCond<float>([] (float x,float y,float)->float {return (x<=0.5);});
		case 4:
			return new FunctionInitialCond<float>([] (float x,float y,float)->float {return (y<=0.5);});
		case 5:
			return new FunctionInitialCond<float>([] (float x,float y,float)->float {return (x<=0.5 && y<=0.5)||(x>=0.5 && y>=0.5);});
		case 6:
			return new FunctionInitialCond<float>([] (float x,float y, float)->float {return x;});
		case 7:
			return new FunctionInitialCond<float>([] (float x,float y, float)->float {return y;});
		case 8:
			return new FunctionInitialCond<float>([] (float x,float y, float)->float {return x*y;});
		case 9:
			return new FunctionInitialCond<float>([] (float x,float y,float)->float {return utils::abs<float>(cos(2*3.14*5*x)*cos(2*3.14*5*y));});
		case 10:
			return new FunctionInitialCond<float>([] (float x,float y,float)->float {return utils::abs<float>(sin(2*3.14*5*x)*sin(2*3.14*5*y));});
		case 11:
			return new CircleInitialCond<float>(0.05,0.5,0.5,0.5);
		case 12:
			return new CircleInitialCond<float>(0.05,0.75,0.75,0.5);
		case 13:
			{
				_codeSent = false;
				emit showCodeEditor(PythonCodeType::PYTHON_FUNC);
				while(!_codeSent)
					std::this_thread::sleep_for(std::chrono::milliseconds(20));

				return new PythonFuncInitialCond<float>(_code.toStdString());
			}
		case 14:
			{
				_codeSent = false;
				emit showCodeEditor(PythonCodeType::PYTHON_CODE);
				while(!_codeSent)
					std::this_thread::sleep_for(std::chrono::milliseconds(20));

				return new PythonCodeInitialCond<float>(_code.toStdString());
			}
		default:
			log_console->errorStream() << "[InitialCondFactory] Unknown initial condition : " << i;
			exit(EXIT_FAILURE);
	}
}

const QString InitialCondFactory::getInitialCondName(int i) {
	switch (i) {
		case 0:
			return "Zero";
		case 1:
			return "Half";
		case 2:
			return "One";
		case 3:
			return "Half plane X";
		case 4:
			return "Half plane Y";
		case 5:
			return "Checkerboard";
		case 6:
			return "Gradient X";
		case 7:
			return "Gradient Y";
		case 8:
			return "Gradient";
		case 9:
			return "Abs. cosine";
		case 10:
			return "Abs. sine";
		case 11:
			return "Circle centered";
		case 12:
			return "Circle offset";
		case 13:
			return "Python function";
		case 14:
			return "Python code";
		default:
			return "";
	}
}

void InitialCondFactory::codeSubmitted(const QString &code) {
	_code = code;
	_codeSent = true;
}
