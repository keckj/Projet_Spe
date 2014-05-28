
#ifndef GRID3D_H
#define GRID3D_H

#include "grid.hpp"

template <typename T>
class Grid3D : public Grid<T> {

	public:
		Grid3D(const Grid3D &grid); //ne copie pas les données

		explicit Grid3D(double realWidth_, double realHeight_, double realLength_,
				unsigned int width_, unsigned int height_, unsigned int length_,
				bool allocate = true);
		
		explicit Grid3D(double realWidth_, double realHeight_, double realLength_,
				double dh_,
				bool allocate = true);
		
		explicit Grid3D(unsigned int width_, unsigned int height_, unsigned int length_,
				double dh_,
				bool allocate = true);
		
		~Grid3D();
		
		//ne copie pas les données
		Grid3D<T> *clone() const;

		unsigned long size() const;
		unsigned long bytes() const;
		
		void allocateOnCpu();
};

#include "grid3D.tpp"

#endif
