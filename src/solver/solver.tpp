
#include "log.hpp"
using namespace log4cpp;

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
		display(res);

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

	T dt = system.computeOptimalTimestep();
	unsigned int i = 0;
	unsigned int maxSteps = maxTime/dt;
	for (T t = 0; t < maxTime; t+=dt) {
		res = system.step(dt);

		writeData(res);
		display(res);
		
		if(i%(maxSteps/20) == 0) 
			log_console->infoStream() << "\t" << 100.0*t/maxTime << "%\t(t=" << t << "/" << maxTime << ")";


		if(stopCond())
			break;

		i++;
	}
	
	log_console->infoStream() << "\t" << 100 <<  "%\t(t=" << maxTime/dt*dt << "/" << maxTime << ")";
}
