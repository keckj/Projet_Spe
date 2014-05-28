
//ne copie pas les données, pointeur NULL
template <typename T>
Grid2D<T>::Grid2D(const Grid2D<T> &grid) :
	Grid<T>(grid)
{
}
		
//ne copie pas les données, pointeur NULL
template <typename T>
Grid2D<T> *Grid2D<T>::clone() const {
	Grid2D<T> *grid = new Grid2D<T>(*this);
	return grid;
}


template <typename T>
Grid2D<T>::Grid2D(double realWidth_, double realHeight_,
		unsigned int width_, unsigned int height_,
		bool allocate) :
	Grid<T>(realWidth_, realHeight_, 0.0,
			width_, height_, 0u,
			2u, allocate)
{
	assert(realWidth_ > 0.0);
	assert(realHeight_ > 0.0);
	assert(width_ > 0u);
	assert(height_ > 0u);

	if(allocate)
		allocateOnCpu();
}

template <typename T>
Grid2D<T>::Grid2D(double realWidth_, double realHeight_, 
		double dh_, bool allocate) : 
	Grid<T>(realWidth_, realHeight_, 0.0,
			dh_,
			2u, allocate)
{
	assert(realWidth_ > 0.0);
	assert(realHeight_ > 0.0);
	assert(dh_ > 0.0);
}

template <typename T>
Grid2D<T>::Grid2D(unsigned int width_, unsigned int height_,
		double dh_,
		bool allocate) :
	Grid<T>(width_, height_, 0u,
			dh_,
			2u, allocate)
{
	assert(width_ > 0u);
	assert(height_ > 0u);
	assert(dh_ > 0.0);
}

template <typename T>
Grid2D<T>::~Grid2D() {
}

template <typename T>
T Grid2D<T>::operator()(unsigned int i, unsigned int j) const {
	return this->_data[j*this->_width+i];
} 

template <typename T>
T& Grid2D<T>::operator()(unsigned int i, unsigned int j) {
	return this->_data[j*this->_width+i];
} 

template <typename T>
unsigned long Grid2D<T>::size() const {
	return this->_width*this->_height;
}

template <typename T>
unsigned long Grid2D<T>::bytes() const {
	return this->_width*this->_height*sizeof(T);
}

template <typename T>
void Grid2D<T>::allocateOnCpu() {
	if(!this->_isAllocated) {
		this->_data = new T[this->_width*this->_height];

		if(this->_data == 0) {
			log_console->errorStream() << "Failed to allocate 2D grid !";
			exit(EXIT_FAILURE);
		}

		this->_isAllocated = true;
	}
	else {
		log_console->warnStream() << "Trying to allocate an already allocated 2D grid !";
	}
}
