

#ifndef GRID1D_H
#define GRID1D_H

#include "grid.hpp"

template <typename T>
class Grid1D : public Grid<T> {

	public:
		explicit Grid1D(const std::string &src);
		explicit Grid1D(const Grid1D &grid, bool copyPointer);
		explicit Grid1D(T realWidth_,  unsigned int width_, bool allocate = true);
		explicit Grid1D(T realWidth_, T dh_, bool allocate = true);
		explicit Grid1D(unsigned int width_, T dh_, bool allocate = true);
		~Grid1D();
		
		T operator()(unsigned int i) const; 
		T& operator()(unsigned int i); 
};

#include "grid1D.tpp"

#endif
