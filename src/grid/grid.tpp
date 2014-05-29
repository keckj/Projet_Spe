
#include <cassert>
#include <typeinfo>
#include <fstream>

#include "log.hpp"
#include "utils.hpp"

using namespace log4cpp;

template <typename T>
Grid<T>::Grid() :
	_realWidth(0), _realHeight(0), _realLength(0),
	_width(0), _height(0), _length(0),
	_dh(0),
	_dim(0),
	_isAllocated(false),
	_data(0)
{
}
		

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
Grid<T>::Grid(T realWidth_, T realHeight_, T realLength_,
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
Grid<T>::Grid(T realWidth_, T realHeight_, T realLength_,
		T dh_,
		unsigned int dim_, bool allocate) :
	_realWidth(realWidth_), _realHeight(realHeight_), _realLength(realLength_),
	_width(realWidth_/dh_), _height(realHeight_/dh_), _length(realLength_/dh_),
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
		T dh_,
		unsigned int dim_, bool allocate) :
	_realWidth(width_*dh_), _realHeight(height_*dh_), _realLength(length_*dh_),
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
T Grid<T>::realWidth() const {
	return _realWidth;
}
template <typename T>
T Grid<T>::realHeight() const {
	return _realHeight;
}
template <typename T>
T Grid<T>::realLength() const {
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
T * Grid<T>::data() const {
	return _data;
}
template <typename T>
T Grid<T>::dh() const {
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
		
template <typename T>
void Grid<T>::save(const std::string &dst) {
	std::ofstream file(dst, std::ios::out | std::ios::trunc);

	if(!file.good()) {
		log_console->errorStream() << "Failed to open file " << dst << " !";
		file.close();
		exit(EXIT_FAILURE);
	}
	
	size_t typeHash = typeid(T).hash_code();
	size_t typeNameLength = strlen(typeid(T).name());
	file.write((char*) &typeHash, sizeof(size_t)); 
	file.write((char*) &typeNameLength, sizeof(size_t)); 
	file.write(typeid(T).name(), typeNameLength*sizeof(char)); 
	file.write((char*) &this->_realWidth, sizeof(T)); 
	file.write((char*) &this->_realHeight, sizeof(T)); 
	file.write((char*) &this->_realLength, sizeof(T)); 
	file.write((char*) &this->_width, sizeof(unsigned int)); 
	file.write((char*) &this->_height, sizeof(unsigned int)); 
	file.write((char*) &this->_length, sizeof(unsigned int)); 
	file.write((char*) &this->_dh, sizeof(T)); 
	file.write((char*) &this->_dim, sizeof(unsigned int)); 
	file.write((char*) &this->_isAllocated, sizeof(bool)); 
	if(this->_isAllocated)
		file.write((char*) _data, this->bytes());
	
	if(!file.good()) {
		log_console->errorStream() << "Failed to write file " << dst << " !";
		file.close();
		exit(EXIT_FAILURE);
	}

	file.close();
}

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
