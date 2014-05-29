
#ifndef SIMPLESYSTEM_H
#define SIMPLESYSTEM_H

#include "system.hpp"

template <typename T>
class SimpleSystem : public System<T> {

public:
	SimpleSystem(std::map<std::string, Grid<T> *> *initialCond,
			T epsilon_, T k_, T d_,
			T mu_1_, T mu_2_,
			T alpha_1_, T alpha_2_);
	
	~SimpleSystem();

	std::map<std::string, Grid<T> *> *step(T dt) override;
	T computeOptimalTimestep() override;

private:
	T F(unsigned int i, unsigned int j, unsigned int k);
	T G(unsigned int i, unsigned int j, unsigned int k);
	T L(unsigned int i, unsigned int j, unsigned int k);

	T _epsilon, _k, _d;
	T _mu_1, _mu_2;
	T _alpha_1, _alpha_2;
};

#include "simpleSystem.tpp"

#endif /* end of include guard: SIMPLESYSTEM_H */
