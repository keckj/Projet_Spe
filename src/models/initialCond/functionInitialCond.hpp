
#ifndef FUNCTIONINITIALCOND_H
#define FUNCTIONINITIALCOND_H

#include "initialCond.hpp"
#include <functional>

template <typename T>
class FunctionInitialCond : public InitialCond<T> {

//std::function is a general-purpose polymorphic function wrapper
//=> lambda expression, function pointer, functor

public:
	FunctionInitialCond(std::function<T(T,T,T)> func);
	~FunctionInitialCond();

private:
	T F(T x, T y, T z) const;

	std::function<T(T,T,T)> _func;
};


template <typename T>
FunctionInitialCond<T>::FunctionInitialCond(std::function<T(T,T,T)> func) : 
	_func(func) {}

template <typename T>
FunctionInitialCond<T>::~FunctionInitialCond() {}

template <typename T>
T FunctionInitialCond<T>::F(T x, T y, T z) const {
	return _func(x,y,z);
}

#endif /* end of include guard: FUNCTIONINITIALCOND_H */
