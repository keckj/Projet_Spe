
#ifndef GRID2D_H
#define GRID2D_H

#include "grid.hpp"

template <typename T>
class Grid2D : public Grid<T> {

	public:
		
		Grid2D(const std::string &src);
		Grid2D(const Grid2D &grid, bool copyPointer = true);

		explicit Grid2D(T realWidth_, T realHeight_,
				unsigned int width_, unsigned int height_,
				bool allocate_ = true);
		
		explicit Grid2D(T realWidth_, T realHeight_, 
				T dh_, bool allocate = true);
		
		explicit Grid2D(unsigned int width_, unsigned int height_,
				T dh_, bool allocate_ = true);
		
		~Grid2D();
		
		T operator()(unsigned int i, unsigned int j) const; 
		T& operator()(unsigned int i, unsigned int j); 
};

#include "grid2D.tpp"

#endif
