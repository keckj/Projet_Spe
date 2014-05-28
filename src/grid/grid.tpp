
#include <cassert>
#include "log.hpp"

using namespace log4cpp;

template <typename T>
Grid<T>::Grid(double realWidth_, double realHeight_, double realLength_,
		unsigned int width_, unsigned int height_, unsigned int length_,
		unsigned int dim_, bool allocate) :
	_realWidth(realWidth_), _realHeight(realHeight_), _realLength(realLength_),
	_width(width_), _height(height_), _length(length_),
	_dh(realWidth_/width_),
	_dim(dim_),
	_isAllocated(false),
	_data(0)
{
	assert(    (_dim == 1 && _realWidth != 0 && _realHeight == 0 && _realLength == 0)
			|| (_dim == 2 && _realWidth != 0 && _realHeight != 0 && _realLength == 0)
			|| (_dim == 3 && _realWidth != 0 && _realHeight != 0 && _realLength != 0));

}

template <typename T>
Grid<T>::Grid(double realWidth_, double realHeight_, double realLength_,
		double dh_,
		unsigned int dim_, bool allocate) :
	_realWidth(realWidth_), _realHeight(realHeight_), _realLength(realLength_),
	_width(realWidth_/dh_), _height(realHeight_/dh_), _length(realLength_/dh_),
	_dh(dh_),
	_dim(dim_),
	_isAllocated(false),
	_data(0)
{
	assert(    (_dim == 1 && _realWidth != 0 && _realHeight == 0 && _realLength == 0)
			|| (_dim == 2 && _realWidth != 0 && _realHeight != 0 && _realLength == 0)
			|| (_dim == 3 && _realWidth != 0 && _realHeight != 0 && _realLength != 0));

	if(allocate)
		allocateOnCpu();
}

template <typename T>
Grid<T>::Grid(unsigned int width_, unsigned int height_, unsigned int length_,
		double dh_,
		unsigned int dim_, bool allocate) :
	_realWidth(width_*dh), _realHeight(height_*dh), _realLength(length_*dh),
	_width(width_), _height(height_), _length(length_),
	_dh(dh_),
	_dim(dim_),
	_isAllocated(false),
	_data(0)
{
	assert(    (_dim == 1 && _realWidth != 0 && _realHeight == 0 && _realLength == 0)
			|| (_dim == 2 && _realWidth != 0 && _realHeight != 0 && _realLength == 0)
			|| (_dim == 3 && _realWidth != 0 && _realHeight != 0 && _realLength != 0));

	if(allocate)
		allocateOnCpu();
}

template <typename T>
Grid<T>::~Grid() {
	if(_isAllocated)
		delete [] _data;
}

template <typename T>
double Grid<T>::realWidth() const {
	return _realWidth;
}
template <typename T>
double Grid<T>::realHeight() const {
	return _realHeight;
}
template <typename T>
double Grid<T>::realLength() const {
	return _realLength;
}

template <typename T>
unsigned int Grid<T>::width() const {
	return _width;
}
template <typename T>
unsigned int Grid<T>::height() const {
	return _height;
}
template <typename T>
unsigned int Grid<T>::length() const {
	return _length;
}

template <typename T>
unsigned int Grid<T>::dim() const {
	return _dim;
}
template <typename T>
bool Grid<T>::isAllocated() const {
	return _isAllocated;
}
template <typename T>
T * Grid<T>::data() {
	return _data;
}
template <typename T>
double Grid<T>::dh() {
	return _dh;
}

template <typename T>
T Grid<T>::operator()(unsigned int i, unsigned int j, unsigned int k) const {
	return data[k*_height*_width+j*_width+i];
} 

template <typename T>
T& Grid<T>::operator()(unsigned int i, unsigned int j, unsigned int k) {
	return data[k*_height*_width+j*_width+i];
} 

template <typename T>
T Grid<T>::operator[](unsigned int n) const {
	return data[n];
}

template <typename T>
T& Grid<T>::operator[](unsigned int n) {
	return data[n];
}

		
