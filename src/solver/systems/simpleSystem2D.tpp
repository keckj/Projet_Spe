
using namespace utils;

template <typename T>
SimpleSystem2D<T>::SimpleSystem2D(std::map<std::string, Grid2D<T> *> *initialCond,
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
SimpleSystem2D<T>::~SimpleSystem2D() {
}

template <typename T>
std::map<std::string, Grid2D<T> *> *SimpleSystem2D<T>::step(T dt) {

	Grid2D<T> e = this->grids->at("e");
	Grid2D<T> r = this->grids->at("r");
	Grid2D<T> old_e = this->grids_old->at("e");
	Grid2D<T> old_r = this->grids_old->at("r");

	for (unsigned int j = 0; j < e.height(); j++) {
		for (unsigned int i = 0; i < e.width(); i++) {
			e(i,j) = old_e(i,j) + dt * (L(i,j) + F(i,j)); 
			r(i,j) = old_r(i,j) + dt * G(i,j); 
		}
	}

	std::swap(this->_grids, this->_old_grids);

	return this->grids;
}

template <typename T>
T SimpleSystem2D<T>::computeOptimalTimestep() {
	T dh = this->_dh;
	T lhs = SQUARE(dh) / 
		(4*_d + _k*SQUARE(dh)/4 *(SQUARE(_alpha_1+1) + 1));

	T rhs = 1 /
		(_epsilon + _mu_1/_mu_2 * SQUARE(dh)/4 * SQUARE(_alpha_1 + 1));

	return min(lhs, rhs);
}

template <typename T>
T SimpleSystem2D<T>::F(unsigned int i, unsigned int j) {
	Grid2D<T> e = this->old_grids->at("e");
	Grid2D<T> r = this->old_grids->at("r");

	T E_ij = e(i,j);
	T R_ij = r(i,j);

	return _k*E_ij*(E_ij - _alpha_1)*(E_ij - 1) - E_ij*R_ij;
}

template <typename T>
T SimpleSystem2D<T>::G(unsigned int i, unsigned int j) {
	Grid2D<T> e = this->old_grids->at("e");
	Grid2D<T> r = this->old_grids->at("r");

	T E_ij = e(i,j);
	T R_ij = r(i,j);

	return (_epsilon + _mu_1*R_ij/(E_ij + _mu_2)) * (-R_ij - _k*E_ij*(E_ij-_alpha_2-1));
}

template <typename T>
T SimpleSystem2D<T>::L(unsigned int i, unsigned int j) {

	Grid2D<T> e = this->old_grids->at("e");

	T dh = this->_dh;

	T E_ij = e(i,j);
	T E_h = e(i == 0 ? 0 : i - 1,j);
	T E_b = e(i == e.width() - 1 ? e.width() - 1 : i + 1, j);
	T E_g = e(i, j == 0 ? 0 : j - 1);
	T E_d = e(i, j == e.height() - 1 ? e.height() - 1 : j + 1);

	return _d/SQUARE(dh) * (E_h + E_b + E_g + E_d - 4*E_ij);
}

