
template <typename T>
Solver<T>::Solver() {
}

template <typename T>
Solver<T>::~Solver() {
}

template <typename T>
void Solver<T>::solve(System<T> &system, unsigned int maxSteps) {
	T dt = system.computeOptimalTimestep();

	for (unsigned int i = 0; i < maxSteps; i++) {
		display(system.step(dt));
		if(stopCond())
			break;
	}
}
