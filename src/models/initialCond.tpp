
#include "initialCond.hpp"

template <typename T>
InitialCond<T>::InitialCond() {
}

template <typename T>
InitialCond<T>::~InitialCond() {
}

template <typename T>
void InitialCond<T>::initializeGrid(Grid<T> *grid) const {
	initializeSubGrid(
			grid->data(), 
			0u,0u,0u, 
			grid->width(), grid->height(), grid->length(),
			grid->width(), grid->height(), grid->length());
}

template <typename T>
void InitialCond<T>::initializeSubGrid(
		T *data, 
		unsigned int offsetX, unsigned int offsetY, unsigned int offsetZ,
		unsigned int subGridWidth, unsigned int subGridHeight, unsigned int subGridLength,
		unsigned int gridWidth, unsigned int gridHeight, unsigned int gridLength) const
{
	T x,y,z;

	for (unsigned int k = 0; k < subGridLength; k++) {
		for (unsigned int j = 0; j < subGridHeight; j++) {
			for (unsigned int i = 0; i < subGridWidth; i++) {
				unsigned long offset = k*subGridHeight*subGridWidth + j*subGridWidth + i;
				x = (gridWidth -1u==0u ? T(0.5) : (T)(offsetX + i) / (T)(gridWidth -1));
				y = (gridHeight-1u==0u ? T(0.5) : (T)(offsetY + j) / (T)(gridHeight-1));
				z = (gridLength-1u==0u ? T(0.5) : (T)(offsetZ + z) / (T)(gridLength-1));
				data[offset] = F(x,y,z);
			}
		}
	}
}
