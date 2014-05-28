

#ifndef GRID1D_H
#define GRID1D_H

#include "grid.hpp"

template <typename T>
class Grid1D : public Grid<T> {

	public:
		Grid1D(const Grid1D &grid); //ne copie pas les données
		explicit Grid1D(double realWidth_,  unsigned int width_, bool allocate = true);
		explicit Grid1D(double realWidth_, double dh_, bool allocate = true);
		explicit Grid1D(unsigned int width_, double dh_, bool allocate = true);
		~Grid1D();
		
		//ne copie pas les données
		Grid1D<T> *clone() const;

		unsigned long size() const;
		unsigned long bytes() const;
		
		T operator()(unsigned int i) const; 
		T& operator()(unsigned int i); 
		
		void allocateOnCpu();
		
};

#include "grid1D.tpp"

#endif
