
//ne copie pas les données, pointeur NULL
template <typename T>
Grid3D<T>::Grid3D(const Grid3D<T> &grid) :
	Grid<T>(grid)
{
}
		
//ne copie pas les données, pointeur NULL
template <typename T>
Grid3D<T> *Grid3D<T>::clone() const {
	Grid3D<T> *grid = new Grid3D<T>(*this);
	return grid;
}

template <typename T>
Grid3D<T>::Grid3D(double realWidth_, double realHeight_, double realLength_,
		unsigned int width_, unsigned int height_, unsigned int length_,
		bool allocate) :
	Grid<T>(realWidth_, realHeight_, realLength_,
			width_, height_, length_,
			3u, allocate)
{
	assert(realWidth_ > 0.0);
	assert(realHeight_ > 0.0);
	assert(realLength_ > 0.0);
	assert(width_ > 0u);
	assert(height_ > 0u);
	assert(length_ > 0u);
	
	if(allocate)
		allocateOnCpu();
}

template <typename T>
Grid3D<T>::Grid3D(double realWidth_, double realHeight_, double realLength_,
		double dh_, bool allocate) : 
	Grid<T>(realWidth_, realHeight_, realLength_,
			dh_,
			3u, allocate)
{
	assert(realWidth_ > 0.0);
	assert(realHeight_ > 0.0);
	assert(realLength_ > 0.0);
	assert(dh_ > 0.0);
}

template <typename T>
Grid3D<T>::Grid3D(unsigned int width_, unsigned int height_, unsigned int length_,
		double dh_,
		bool allocate) :
	Grid<T>(width_, height_, length_,
			dh_,
			3u, allocate)
{
	assert(width_ > 0u);
	assert(height_ > 0u);
	assert(length_ > 0u);
	assert(dh_ > 0.0);
}

template <typename T>
Grid3D<T>::~Grid3D() {
}

template <typename T>
unsigned long Grid3D<T>::size() const {
	return this->_width*this->_height*this->_length;
}

template <typename T>
unsigned long Grid3D<T>::bytes() const {
	return this->_width*this->_height*this->_length*sizeof(T);
}

template <typename T>
void Grid3D<T>::allocateOnCpu() {
	if(!this->_isAllocated) {
		this->_data = new T[this->_width*this->_height*this->_length];

		if(this->_data == 0) {
			log_console->errorStream() << "Failed to allocate grid 3D !";
			exit(EXIT_FAILURE);
		}

		this->_isAllocated = true;
	}
	else {
		log_console->warnStream() << "Trying to allocate an already allocated 3D grid !";
	}
}
