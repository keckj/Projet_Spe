
#ifndef FUNCTOR_H
#define FUNCTOR_H

#include <map>
#include <string>

template <typename T>
class Functor {

	public:
		Functor();
		~Functor();

		virtual T operator(std::map<std::string,Grid<T> *> grids, unsigned int i, unsigned int j, unsigned int k) = 0;
}

#endif /* end of include guard: FUNCTOR_H */
