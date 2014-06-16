

#ifndef PYTHONCODEINITIALCOND_H
#define PYTHONCODEINITIALCOND_H

#include <Python.h>
#include "initialCond.hpp"
#include <functional>
#include <sstream>

using namespace log4cpp;

template <typename T>
class PythonCodeInitialCond : public InitialCond<T> {

public:
	PythonCodeInitialCond(const std::string &expr);
	~PythonCodeInitialCond();

	static unsigned long getCurrentId();

private:
	T F(T x, T y, T z) const;

	unsigned int hash(const char *str);

	PyObject *_expression;
	char *_args;

	static unsigned long id;
};

template <typename T>
unsigned long PythonCodeInitialCond<T>::id = 0ul;
	
template <typename T>
unsigned long PythonCodeInitialCond<T>::getCurrentId() {
	return id;
}

template <typename T>
PythonCodeInitialCond<T>::PythonCodeInitialCond(const std::string &expr) :
	InitialCond<T>(true), _expression(0), _args(0) {
	
	std::stringstream funcNameSS;
	funcNameSS << "F" << id++;
	std::string funcName = funcNameSS.str();

	PyRun_SimpleString(expr.c_str());

	log_console->infoStream() << "Python code :\n" << expr;
    
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
PythonCodeInitialCond<T>::~PythonCodeInitialCond() {
	delete [] _args;
}

template <typename T>
T PythonCodeInitialCond<T>::F(T x, T y, T z) const {
	return T(PyFloat_AsDouble(PyObject_CallFunction(_expression, _args, x,y,z)));
}

template <typename T>
unsigned int PythonCodeInitialCond<T>::hash(const char *str)
{
    unsigned int h = 0;
    while (*str)
       h = h << 1 ^ *str++;
    return h;
}

#endif /* end of include guard: PYTHONCODEINITIALCOND_H */
