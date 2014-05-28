
template <typename T>
Grid1D<T>::Grid1D(double realWidth_, unsigned int width_, bool allocate) :
	Grid<T>(realWidth_, 0.0, 0.0,
			width_, 0u, 0u,
			1u, allocate)
{
	assert(realWidth_ > 0.0);
	assert(width_ > 0u);

	if(allocate)
		allocateOnCpu();
}

template <typename T>
Grid1D<T>::Grid1D(double realWidth_, double dh_, bool allocate) : 
	Grid<T>(realWidth_, 0.0, 0.0,
			dh_,
			1u, allocate)
{
	assert(realWidth_ > 0.0);
	assert(dh_ > 0.0);
}

template <typename T>
Grid1D<T>::Grid1D(unsigned int width_, double dh_, bool allocate) :
	Grid<T>(width_, 0u, 0u,
			dh_,
			1u, allocate)
{
	assert(width_ > 0u);
	assert(dh_ > 0.0);
}

template <typename T>
Grid1D<T>::~Grid1D() {
}

template <typename T>
T Grid1D<T>::operator()(unsigned int i) const {
	return this->_data[i];
} 

template <typename T>
T& Grid1D<T>::operator()(unsigned int i) {
	return this->_data[i];
} 

template <typename T>
unsigned long Grid1D<T>::size() const {
	return this->_width;
}

template <typename T>
unsigned long Grid1D<T>::bytes() const {
	return this->_width*sizeof(T);
}

template <typename T>
void Grid1D<T>::allocateOnCpu() {
	if(!this->_isAllocated) {
		this->_data = new T[this->_width];

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
