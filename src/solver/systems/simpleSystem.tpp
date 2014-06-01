
#include <algorithm>
#include <thread>
#include <vector>

using namespace utils;

template <typename T>
SimpleSystem<T>::SimpleSystem(const std::map<std::string, Grid<T> *> initialCond,
		T epsilon_, T k_, T d_,
		T mu_1_, T mu_2_,
		T alpha_1_, T alpha_2_) :
	System<T>(initialCond),
	_epsilon(epsilon_), _k(k_), _d(d_),
	_mu_1(mu_1_), _mu_2(mu_2_),
	_alpha_1(alpha_1_), _alpha_2(alpha_2_)
{
}

template <typename T>
SimpleSystem<T>::~SimpleSystem() {
}

template <typename T>
std::map<std::string, Grid<T> *> *SimpleSystem<T>::step(T dt) {

	Grid<T> *e = this->grids->at("e");
	Grid<T> *r = this->grids->at("r");
	Grid<T> *old_e = this->grids_old->at("e");
	Grid<T> *old_r = this->grids_old->at("r");
	
	unsigned int width = e->width();
	unsigned int height = e->height();
	unsigned int length = e->length();

	unsigned int nThread = 8;
	unsigned long workSize = e->size();	

	unsigned long subworkSize = workSize/nThread;
	unsigned long lastWork = workSize%nThread;

	std::list<std::thread> threads;
	for (unsigned long i = 0; i < nThread; i++) {
		threads.push_back(std::thread(&SimpleSystem<T>::subStep, this, dt, i*subworkSize, subworkSize));
	}
	
	subStep(dt, nThread*subworkSize, lastWork);
	
	for (auto& it : threads) {
		it.join();
	}

	std::swap(this->grids, this->grids_old);

	return this->grids;
}

template <typename T>
void SimpleSystem<T>::subStep(T dt, unsigned long offset, unsigned long subworkSize) {
	Grid<T> *e = this->grids->at("e");
	Grid<T> *r = this->grids->at("r");
	Grid<T> *old_e = this->grids_old->at("e");
	Grid<T> *old_r = this->grids_old->at("r");

	unsigned int width = e->width();
	unsigned int height = e->height();
	unsigned int length = e->length();
	unsigned int i,j;
	T *E_1 = old_e->data();
	T *E_2 = e->data();
	T *R_1 = old_r->data();
	T *R_2 = r->data();
	
	T dh = this->_dh;

	for (unsigned long id = offset; id < offset+subworkSize; id++) {
		j = (id % (width*height))/width;
		i = id % width;
		(*e)(i,j,0) = (*old_e)(i,j,0) + dt * (L(i,j,0) + F(i,j,0)); 
		(*r)(i,j,0) = (*old_r)(i,j,0) + dt * G(i,j,0); 

		//T E = E_1[id];
		//T R = R_1[id];

		//T _F = -_k*E*(E - _alpha_1)*(E - 1.0f) - E*R;
		//T _G = (_epsilon + _mu_1*R/(E + _mu_2)) * (-R - _k*E*(E-_alpha_2-1.0f));
		//T _L =  _d/(dh*dh) * (
		//+ E_1[i == 0 ? id : id-1]
		//+ E_1[i == width-1 ? id : id+1]
		//+ E_1[j == 0 ? id : id-width ]
		//+ E_1[j == height-1 ? id : id+width]
		//- 4*E
		//);

		//E_2[id] = E + dt*(_L+_F);
		//R_2[id] = R + dt*_G;
	}
}

template <typename T>
T SimpleSystem<T>::computeOptimalTimestep() {
	T dh = this->_dh;
	T lhs = SQUARE(dh) / 
		(4*_d + _k*SQUARE(dh)/4 *(SQUARE(_alpha_1+1) + 1));

	T rhs = 1 /
		(_epsilon + _mu_1/_mu_2 * SQUARE(dh)/4 * SQUARE(_alpha_1 + 1));

	return 0.95*std::min(lhs, rhs);
}

template <typename T>
T SimpleSystem<T>::F(unsigned int i, unsigned int j, unsigned int k) {
	Grid<T> *e = this->grids_old->at("e");
	Grid<T> *r = this->grids_old->at("r");

	T E_ijk = (*e)(i,j,k);
	T R_ijk = (*r)(i,j,k);

	return -_k*E_ijk*(E_ijk - _alpha_1)*(E_ijk - 1) - E_ijk*R_ijk;
}

template <typename T>
T SimpleSystem<T>::G(unsigned int i, unsigned int j, unsigned int k) {
	Grid<T> *e = this->grids_old->at("e");
	Grid<T> *r = this->grids_old->at("r");

	T E_ijk = (*e)(i,j,k);
	T R_ijk = (*r)(i,j,k);

	return (_epsilon + _mu_1*R_ijk/(E_ijk + _mu_2)) * (-R_ijk - _k*E_ijk*(E_ijk-_alpha_2-1));
}

template <typename T>
T SimpleSystem<T>::L(unsigned int i, unsigned int j, unsigned int k) {

	Grid<T> *e = this->grids_old->at("e");

	T dh = this->_dh;

	T E_ijk = (*e)(i,j,k);
	T E_left = (*e)(i == 0 ? 0 : i - 1, j, k);
	T E_right = (*e)(i == e->width()-1 ? e->width()-1 : i + 1, j, k);
	T E_up = (*e)(i, j == 0 ? 0 : j - 1, k);
	T E_down = (*e)(i, j == e->height()-1 ? e->height()-1 : j + 1, k);
	//T E_front = (*e)(i, j, k == 0 ? 0 : k - 1);
	//T E_back = (*e)(i, j, k == e->length() - 1 ? e->length() - 1 : k + 1);

	return _d/(dh*dh) * (E_left + E_right + E_up + E_down - 4*E_ijk);
}

