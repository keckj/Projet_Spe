
#ifndef GRID3D_H
#define GRID3D_H

#include "grid.hpp"

template <typename T>
class Grid3D : public Grid<T> {

	public:
		explicit Grid3D(const std::string &src);
		explicit Grid3D(const Grid3D &grid, bool copyPointer = true);

		explicit Grid3D(T realWidth_, T realHeight_, T realLength_,
				unsigned int width_, unsigned int height_, unsigned int length_,
				bool allocate = true);
		
		explicit Grid3D(T realWidth_, T realHeight_, T realLength_,
				T dh_,
				bool allocate = true);
		
		explicit Grid3D(unsigned int width_, unsigned int height_, unsigned int length_,
				T dh_,
				bool allocate = true);
		
		~Grid3D();
};

#include "grid3D.tpp"

#endif
