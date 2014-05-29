
#include <algorithm>
using namespace utils;

template <typename T>
SimpleSystem<T>::SimpleSystem(std::map<std::string, Grid<T> *> *initialCond,
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

	for (unsigned int k = 0; k < e->length(); k++) {
		for (unsigned int j = 0; j < e->height(); j++) {
			for (unsigned int i = 0; i < e->width(); i++) {
				(*e)(i,j,k) = (*old_e)(i,j,k) + dt * (L(i,j,k) + F(i,j,k)); 
				(*e)(i,j,k) = (*old_r)(i,j,k) + dt * G(i,j,k); 
			}
		}
	}

	std::swap(this->grids, this->grids_old);

	return this->grids;
}

template <typename T>
T SimpleSystem<T>::computeOptimalTimestep() {
	T dh = this->_dh;
	T lhs = SQUARE(dh) / 
		(4*_d + _k*SQUARE(dh)/4 *(SQUARE(_alpha_1+1) + 1));

	T rhs = 1 /
		(_epsilon + _mu_1/_mu_2 * SQUARE(dh)/4 * SQUARE(_alpha_1 + 1));

	return std::min(lhs, rhs);
}

template <typename T>
T SimpleSystem<T>::F(unsigned int i, unsigned int j, unsigned int k) {
	Grid<T> *e = this->grids_old->at("e");
	Grid<T> *r = this->grids_old->at("r");

	T E_ijk = (*e)(i,j,k);
	T R_ijk = (*r)(i,j,k);

	return _k*E_ijk*(E_ijk - _alpha_1)*(E_ijk - 1) - E_ijk*R_ijk;
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
	T E_right = (*e)(i == e->width() - 1 ? e->width() - 1 : i + 1, j, k);
	T E_up = (*e)(i, j == 0 ? 0 : j - 1, k);
	T E_down = (*e)(i, j == e->height() - 1 ? e->height() - 1 : j + 1, k);
	T E_front = (*e)(i, j, k == 0 ? 0 : k - 1);
	T E_back = (*e)(i, j, k == e->length() - 1 ? e->length() - 1 : k + 1);

	return _d/SQUARE(dh) * (E_left + E_right + E_up + E_down + E_front + E_back - 6*E_ijk);
}

