
#ifndef SOLVER_H
#define SOLVER_H

#include <map>
#include <string>
#include "grid.hpp" 

template <typename T>
class Solver {
public:
	Solver();
	virtual ~Solver();

	void solve(System<T> &system, unsigned int maxSteps);
	void solve(System<T> &system, T maxTime);

protected:
	virtual void display(std::map<std::string, Grid<T> *> *grids) {};
	virtual void writeData(std::map<std::string, Grid<T> *> *grids) {};

	virtual bool stopCond() {return false;};
};

#include "solver.tpp"

#endif /* end of include guard: SOLVER_H */
