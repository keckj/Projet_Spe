
#ifndef GRID2D_H
#define GRID2D_H

#include "grid.hpp"

template <typename T>
class Grid2D : public Grid<T> {

	public:
		explicit Grid2D(double realWidth_, double realHeight_,
				unsigned int width_, unsigned int height_,
				bool allocate = true);
		
		explicit Grid2D(double realWidth_, double realHeight_, 
				double dh_,
				bool allocate = true);
		
		explicit Grid2D(unsigned int width_, unsigned int height_,
				double dh_,
				bool allocate = true);
		
		~Grid2D();

		unsigned long size() const;
		unsigned long bytes() const;
		
		T operator()(unsigned int i, unsigned int j) const; 
		T& operator()(unsigned int i, unsigned int j); 
		
		void allocateOnCpu();

};

#include "grid2D.tpp"

#endif
