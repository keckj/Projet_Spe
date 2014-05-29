
#include <cassert>
#include <typeinfo>
#include "log.hpp"
#include "utils.hpp"

using namespace log4cpp;
		

template <typename T>
Grid<T>::Grid(const Grid<T> &grid) :
	_realWidth(grid.realWidth()), _realHeight(grid.realHeight()), _realLength(grid.realLength()),
	_width(grid.width()), _height(grid.height()), _length(grid.length()),
	_dh(grid.dh()),
	_dim(grid.dim()),
	_isAllocated(false),
	_data(0)
{
}

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
	assert(    (_dim == 1 && _width > 1 && _height == 1 && _length == 1)
			|| (_dim == 2 && _width > 1 && _height > 1 && _length == 1)
			|| (_dim == 3 && _width > 1 && _height > 1 && _length > 1));
}

template <typename T>
Grid<T>::Grid(double width_, double height_, double length_,
		double dh_,
		unsigned int dim_, bool allocate) :
	_width(width_), _height(height_), _length(length_),
	_width(width_/dh_), _height(height_/dh_), _length(length_/dh_),
	_dh(dh_),
	_dim(dim_),
	_isAllocated(false),
	_data(0)
{
	assert(    (_dim == 1 && _width > 1 && _height == 1 && _length == 1)
			|| (_dim == 2 && _width > 1 && _height > 1 && _length == 1)
			|| (_dim == 3 && _width > 1 && _height > 1 && _length > 1));
}

template <typename T>
Grid<T>::Grid(unsigned int width_, unsigned int height_, unsigned int length_,
		double dh_,
		unsigned int dim_, bool allocate) :
	_width(width_*dh_), _height(height_*dh_), _length(length_*dh_),
	_width(width_), _height(height_), _length(length_),
	_dh(dh_),
	_dim(dim_),
	_isAllocated(false),
	_data(0)
{
	assert(    (_dim == 1 && _width > 1 && _height == 1 && _length == 1)
			|| (_dim == 2 && _width > 1 && _height > 1 && _length == 1)
			|| (_dim == 3 && _width > 1 && _height > 1 && _length > 1));
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
double Grid<T>::dh() const {
	return _dh;
}

template <typename T>
T Grid<T>::operator()(unsigned int i, unsigned int j, unsigned int k) const {
	return _data[k*_height*_width+j*_width+i];
} 

template <typename T>
T& Grid<T>::operator()(unsigned int i, unsigned int j, unsigned int k) {
	return _data[k*_height*_width+j*_width+i];
} 

template <typename T>
T Grid<T>::operator[](unsigned int n) const {
	return _data[n];
}

template <typename T>
T& Grid<T>::operator[](unsigned int n) {
	return _data[n];
}

		
template <typename T>
void Grid<T>::freeOnCpu() {
	delete [] _data;
	_data = 0;
	_isAllocated = false;
}
		
//template <typename T>
//void setData(T *data_);
	//if(isAllocated()) {
		//log_console.warnStream() << "Changing already existing grid data pointer, freeing old memory !";
		//freeOnCpu();
	//}

	//_data = data_;

	//if(_data != NULL)
		//_isAllocated = true;
//}

template <typename T>
std::ostream & operator <<(std::ostream &out, const Grid<T> &grid) {
	out << "Grid<T=" << typeid(T).name() << ">"
		<< "\n\t Dim               : " << grid.dim() << "D"
		<< "\n\t Real size (WxHxL) : " << grid.realWidth() << " x " << grid.realHeight() << " x " << grid.realLength()
		<< "\n\t Size      (WxHxL) : " << grid.width() << " x " << grid.height() << " x " << grid.length()
		<< "\n\t DH                : " << grid.dh()
		<< "\n\t Is allocated ?    : " << (grid.isAllocated() ? "true" : "false")
		<< "\n\t Elements          : " << grid.size()
		<< "\n\t Memory size       : " << utils::toStringMemory(grid.bytes());

	return out;
}
