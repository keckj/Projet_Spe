
#ifndef SOLVER_H
#define SOLVER_H

#include <map>
#include "grid.hpp" 

template <typename T>
class Solver {
public:
	Solver();
	virtual ~Solver();

	void solve(System<T> &system, unsigned int maxSteps);

protected:
	virtual void display(std::map<std::string, Grid<T> *> *grids) {};
	virtual bool stopCond() {return false;};

};

#include "solver.tpp"

#endif /* end of include guard: SOLVER_H */
