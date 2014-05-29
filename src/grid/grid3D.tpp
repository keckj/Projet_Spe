
template <typename T>
Grid3D<T>::Grid3D(const std::string &src) :
	Grid<T>()
{
	std::ifstream file(src, std::ios::in);
	
	if(!file.good()) {
		log_console->errorStream() << "Failed to open file " << src << " !";
		file.close();
		exit(EXIT_FAILURE);
	}
	
	size_t typeHash;
	size_t typeNameLength;
	file.read((char*) &typeHash, sizeof(size_t)); 
	file.read((char*) &typeNameLength, sizeof(size_t)); 
	char *typeName = new char[typeNameLength+1];
	file.read(typeName, typeNameLength*sizeof(char)); 
	typeName[typeNameLength] = '\0';
	
	if(typeHash != typeid(T).hash_code()) {
		log_console->errorStream() << "Loaded a GridXD<" << typeName << "> "
			<< "file but object grid type is GridXD<" << typeid(T).name() << "> "
			<< "(from " << src << ") !";
		file.close();
		exit(EXIT_FAILURE);
	}

	file.read((char*) &this->_realWidth, sizeof(T)); 
	file.read((char*) &this->_realHeight, sizeof(T)); 
	file.read((char*) &this->_realLength, sizeof(T)); 
	file.read((char*) &this->_width, sizeof(unsigned int)); 
	file.read((char*) &this->_height, sizeof(unsigned int)); 
	file.read((char*) &this->_length, sizeof(unsigned int)); 
	file.read((char*) &this->_dh, sizeof(T)); 
	file.read((char*) &this->_dim, sizeof(unsigned int)); 
	file.read((char*) &this->_isAllocated, sizeof(bool)); 
	
	if(!file.good()) {
		log_console->errorStream() << "Failed to parse file " << src << " !";
		file.close();
		exit(EXIT_FAILURE);
	}
	
	if(this->_dim != 3u) {
		log_console->errorStream() << "Loaded a Grid" << this->_dim << "D<" << typeName << "> "
			<< "file but object grid type is Grid3D<" << typeid(T).name() << "> "
			<< "(from " << src << ") !";
		file.close();
		exit(EXIT_FAILURE);
	}
	
	if(this->_isAllocated) {
		this->_data = new T[this->size()];
		file.read((char*) this->_data, this->bytes());
	}
	
	file.close();
}
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
Grid3D<T>::Grid3D(T realWidth_, T realHeight_, T realLength_,
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
Grid3D<T>::Grid3D(T realWidth_, T realHeight_, T realLength_,
		T dh_, bool allocate) : 
	Grid<T>(realWidth_, realHeight_, realLength_,
			dh_,
			3u, allocate)
{
	assert(realWidth_ > 0.0);
	assert(realHeight_ > 0.0);
	assert(realLength_ > 0.0);
	assert(dh_ > 0.0);
	
	if(allocate)
		allocateOnCpu();
}

template <typename T>
Grid3D<T>::Grid3D(unsigned int width_, unsigned int height_, unsigned int length_,
		T dh_,
		bool allocate) :
	Grid<T>(width_, height_, length_,
			dh_,
			3u, allocate)
{
	assert(width_ > 0u);
	assert(height_ > 0u);
	assert(length_ > 0u);
	assert(dh_ > 0.0);
	
	if(allocate)
		allocateOnCpu();
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
