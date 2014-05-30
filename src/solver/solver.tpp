
#include "log.hpp"
#include "plotUtils.hpp"
#include <cstring>
#include <sstream>

using namespace log4cpp;
using namespace utils;

template <typename T>
Solver<T>::Solver() {
}

template <typename T>
Solver<T>::~Solver() {
}

template <typename T>
void Solver<T>::solve(System<T> &system, unsigned int maxSteps) {
	std::map<std::string, Grid<T> *> *res;

	T dt = system.computeOptimalTimestep();
			
	log_console->infoStream() << ":::: Solving system ::::";
	for (unsigned int i = 0; i < maxSteps; i++) {
		res = system.step(dt);

		writeData(res);
		display(res,i);

		if(i%(maxSteps/20) == 0) 
			log_console->infoStream() << "\t" << 100*i/maxSteps << "%\t(it " << i << "/" << maxSteps << ")";

		if(stopCond())
			break;
	}
			
	log_console->infoStream() << "\t" << 100 << "%\t(it " << maxSteps << "/" << maxSteps << ")";
}
	
template <typename T>
void Solver<T>::solve(System<T> &system, T maxTime) {
	std::map<std::string, Grid<T> *> *res;
	log_console->infoStream() << ":::: Solving system ::::";

	std::stringstream ss;

	T dt = system.computeOptimalTimestep();
	unsigned int i = 0;
	unsigned int maxSteps = maxTime/dt;
	for (T t = 0; t < maxTime; t+=dt) {
		res = system.step(dt);

		writeData(res,i,"data/prefix_", "_suffix.dat");
		display(res);
		
		if(i%(maxSteps/20) == 0) 
			log_console->infoStream() << "\t" << 100.0*t/maxTime << "%\t(t=" << t << "/" << maxTime << ")";

		if(stopCond())
			break;
		i++;
	}
	
	log_console->infoStream() << "\t" << 100 <<  "%\t(t=" << maxTime/dt*dt << "/" << maxTime << ")";
}
	
template <typename T>
void Solver<T>::writeData(std::map<std::string, Grid<T> *> *grids, unsigned int i,
		const std::string &prefix, const std::string &suffix) {
	char buffer[1000];

	for(auto it = grids->begin(); it != grids->end(); ++it) {
		sprintf(buffer,"%s%s%i%s",prefix.c_str(),it->first.c_str(),i,suffix.c_str());
		it->second->exportData(buffer);
	}
}
