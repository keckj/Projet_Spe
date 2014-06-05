
#ifndef CIRCLEINITIALCOND_H
#define CIRCLEINITIALCOND_H

#include "initialCond.hpp"

template <typename T>
class CircleInitialCond : public InitialCond<T> {

	public:
		CircleInitialCond(T radius = T(0.1), T centerX = T(0.5), T centerY = T(0.5), T centerZ = T(0.5));
		~CircleInitialCond();

	private:
		T _radius;
		T _centerX, _centerY, _centerZ;

		T F(T x, T y, T z) const override; 
};
		
template <typename T>
CircleInitialCond<T>::CircleInitialCond(T radius, T centerX, T centerY, T centerZ) :
			InitialCond<float>(),
			_radius(radius), _centerX(centerX), _centerY(centerY), _centerZ(centerZ) {}

template <typename T>
CircleInitialCond<T>::~CircleInitialCond() {}

template <typename T>
T CircleInitialCond<T>::F(T x, T y, T z) const {
	T xx = x - _centerX;
	T yy = y - _centerY;
	T zz = z - _centerZ;

	return T(xx*xx + yy*yy + zz*zz <= _radius*_radius);
}

#endif /* end of include guard: CIRCLEINITIALCOND_H */
