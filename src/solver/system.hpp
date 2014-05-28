
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "grid.hpp"
#include <map>

template <typename T>
class System {

	public:
	System(std::map<std::string, Grid<T> *> *initialCond);
	virtual ~System();

	virtual std::map<std::string, Grid<T> *> *step(T dt) = 0;
	virtual T computeOptimalTimestep() = 0;
	
	protected:
		std::map<std::string, Grid<T> *> *grids;
		std::map<std::string, Grid<T> *> *grids_old;
		
		T _dh;
};

#include "system.tpp"

#endif /* end of include guard: SYSTEM_H */

