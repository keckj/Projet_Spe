
#ifndef PYTHONINITIALCOND_H
#define PYTHONINITIALCOND_H

#include <Python.h>
#include "initialCond.hpp"
#include <functional>
#include <sstream>

using namespace log4cpp;

template <typename T>
class PythonFuncInitialCond : public InitialCond<T> {

public:
	PythonFuncInitialCond(const std::string &expr);
	~PythonFuncInitialCond();

private:
	T F(T x, T y, T z) const;

	unsigned int hash(const char *str);

	PyObject *_expression;
	char *_args;
};


template <typename T>
PythonFuncInitialCond<T>::PythonFuncInitialCond(const std::string &expr) :
	_expression(0), _args(0) {
	
	std::stringstream funcNameSS;
	funcNameSS << "func" << hash(expr.c_str());
	std::string funcName = funcNameSS.str();

	std::stringstream pgm;
	pgm << "import math" << std::endl
	<< "def " << funcName << "(x,y,z):" << std::endl
	<< "\t" << expr << std::endl;
	PyRun_SimpleString(pgm.str().c_str());

	log_console->infoStream() << "PYTHON function :\n" << pgm.str();
    
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
PythonFuncInitialCond<T>::~PythonFuncInitialCond() {
	delete [] _args;
}

template <typename T>
T PythonFuncInitialCond<T>::F(T x, T y, T z) const {
	return T(PyFloat_AsDouble(PyObject_CallFunction(_expression, _args, x,y,z)));
}

template <typename T>
unsigned int PythonFuncInitialCond<T>::hash(const char *str)
{
    unsigned int h = 0;
    while (*str)
       h = h << 1 ^ *str++;
    return h;
}

#endif /* end of include guard: PYTHONINITIALCOND_H */
