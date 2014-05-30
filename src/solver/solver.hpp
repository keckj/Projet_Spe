
#ifndef SOLVER_H
#define SOLVER_H

#include <map>
#include <string>
#include <sstream>
#include "grid.hpp" 

template <typename T>
class Solver {
public:
	Solver();
	virtual ~Solver();

	unsigned int solve(System<T> &system, unsigned int maxSteps);
	unsigned int solve(System<T> &system, T maxTime);

protected:
	virtual void display(std::map<std::string, Grid<T> *> *grids) {};

	void writeData(std::map<std::string, Grid<T> *> *grids, unsigned int i,
		const std::string &prefix, const std::string &suffix);

	virtual bool stopCond() {return false;};
};

#include "solver.tpp"

#endif /* end of include guard: SOLVER_H */
