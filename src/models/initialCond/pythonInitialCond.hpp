
#ifndef PYTHONINITIALCOND_H
#define PYTHONINITIALCOND_H

#include "initialCond.hpp"
#include <functional>
#include <sstream>
#include <Python.h>

using namespace log4cpp;

template <typename T>
class PythonInitialCond : public InitialCond<T> {

public:
	PythonInitialCond(const std::string &expr);
	~PythonInitialCond();

private:
	T F(T x, T y, T z) const;

	unsigned int hash(const char *str);

	PyObject *_expression;
	char *_args;
};


template <typename T>
PythonInitialCond<T>::PythonInitialCond(const std::string &expr) :
	_expression(0), _args(0) {
	
	log_console->infoStream() << "INIT !";

	std::stringstream funcNameSS;
	funcNameSS << "func" << hash(expr.c_str());
	std::string funcName = funcNameSS.str();

	std::stringstream pgm;
	pgm << "import math" << std::endl
	<< "def " << funcName << "(x,y,z):" << std::endl
	<< "\treturn " << expr << std::endl;
	PyRun_SimpleString(pgm.str().c_str());

	log_console->infoStream() << "PYTHON pgrm : " << pgm.str();
    
	PyObject*    main_module, * global_dict;
    main_module = PyImport_AddModule("__main__");
    global_dict = PyModule_GetDict(main_module);

	_expression = PyDict_GetItemString(global_dict, funcName.c_str());
	
	_args = new char[4];
	_args[0] = 'f';
	_args[1] = 'f';
	_args[2] = 'f';
	_args[3] = '\0';
}

template <typename T>
PythonInitialCond<T>::~PythonInitialCond() {
	delete [] _args;
}

template <typename T>
T PythonInitialCond<T>::F(T x, T y, T z) const {
	return T(PyFloat_AsDouble(PyObject_CallFunction(_expression, _args, x,y,z)));
}

template <typename T>
unsigned int PythonInitialCond<T>::hash(const char *str)
{
    unsigned int h = 0;
    while (*str)
       h = h << 1 ^ *str++;
    return h;
}

#endif /* end of include guard: PYTHONINITIALCOND_H */
