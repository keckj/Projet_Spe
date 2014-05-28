
#ifndef FUNCTOR_H
#define FUNCTOR_H

#include <vector>

template <typename T>
class Functor {

	public:
		Functor();
		~Functor();

		virtual T operator(std::vector<Grid<T> *>, unsigned int i, unsigned int j, unsigned int k) = 0;
}

#endif /* end of include guard: FUNCTOR_H */
