
#include "domain.hpp"
#include "log.hpp"
#include <cassert>
#include <thread>

using namespace log4cpp;
		
template <typename T, unsigned int N>
MultiBufferedDomain<T,N>::MultiBufferedDomain(
		unsigned int domainWidth_, unsigned int domainHeight_, unsigned int domainLength_,
		unsigned int extraBorderSize_, 
		unsigned int minSplits_,
		InitialCond<float> *initialCondition_) :
	_domainWidth(domainWidth_), 
	_domainHeight(domainHeight_),
	_domainLength(domainLength_),
	_extraBorderSize(extraBorderSize_)
{
	splitDomain(minSplits_, initialCondition_);

	if(_extraBorderSize > 0u)
		linkSubDomains();
}
		
template <typename T, unsigned int N>
MultiBufferedDomain<T,N>::~MultiBufferedDomain() {
	for(MultiBufferedSubDomain<T,N> *dom : _subDomains)
		delete dom;

	log_console->debugStream() << "A multiBufferedDomain has been killed by thread " << std::this_thread::get_id() << " !";
}

template <typename T, unsigned int N>
unsigned int MultiBufferedDomain<T,N>::domainWidth() const {
			return _domainWidth;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedDomain<T,N>::domainHeight() const {
			return _domainHeight;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedDomain<T,N>::domainLength() const {
			return _domainLength;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedDomain<T,N>::extraBorderSize() const {
			return _extraBorderSize;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedDomain<T,N>::nSplits() const {
			return _nSplits;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedDomain<T,N>::splitsX() const {
			return _splitsX;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedDomain<T,N>::splitsY() const {
			return _splitsY;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedDomain<T,N>::splitsZ() const {
			return _splitsZ;
}
		
template <typename T, unsigned int N>
MultiBufferedSubDomain<T,N> *MultiBufferedDomain<T,N>::operator()(unsigned int i, unsigned int j, unsigned int k) {
	return _subDomains[k*_splitsY*_splitsX + j*_splitsX + i];
}

template <typename T, unsigned int N>
MultiBufferedSubDomain<T,N> *MultiBufferedDomain<T,N>::operator[](unsigned int n) {
	return _subDomains[n];
}
		
template <typename T, unsigned int N>
void MultiBufferedDomain<T,N>::splitDomain(unsigned int minSplits, InitialCond<float> *initialCondition_) {
		
	unsigned int splitX=1, splitY=1, splitZ=1;
	unsigned int subdomainWidth=_domainWidth, subdomainHeight=_domainHeight, subdomainLength=_domainLength;

	log_console->debugStream() << "Splitting a " << _domainWidth << " x " << _domainHeight << " x " << _domainLength << " domain (minSplits=" << minSplits << ").";
	
	while(splitX*splitY*splitZ < minSplits) {
		if(subdomainWidth > subdomainHeight && subdomainWidth > subdomainLength) { //on privilégie la découpe en z puis y puis x
			splitX++;
			subdomainWidth = (_domainWidth+splitX-1)/splitX;
		}
		else {
			if(subdomainHeight > subdomainLength) {
				splitY++;
				subdomainHeight = (_domainHeight+splitY-1)/splitY;
			}
			else {
				splitZ++;
				subdomainLength = (_domainLength+splitZ-1)/splitZ;
			}
		}
	}
	
	_splitsX = splitX;
	_splitsY = splitY;
	_splitsZ = splitZ;
	_nSplits = _splitsX*_splitsY*_splitsZ;
	
	log_console->debugStream() << "Splits : " << _nSplits << " (" << _splitsX << "," << _splitsY << "," << _splitsZ << ")";
	log_console->debugStream() << "SubDomains : (" 
		<< _domainWidth/_splitsX << "," 
		<< _domainHeight/_splitsY << ","
		<< _domainLength/_splitsZ << ")"
		<< "\t + O[ ("
		<< _domainWidth/_splitsX + _domainWidth%_splitsX << "," 
		<< _domainHeight/_splitsY + _domainHeight%_splitsY << "," 
		<< _domainLength/_splitsZ + _domainLength%_splitsZ << ") " 
		<< "]";
	

	MultiBufferedSubDomain<T,N> *dom;

	for (unsigned int k = 0; k < _splitsZ; k++) {
		for (unsigned int j = 0; j < _splitsY; j++) {
			for (unsigned int i = 0; i < _splitsX; i++) {
				dom = new MultiBufferedSubDomain<float,N>(
					k*_splitsY*_splitsX + j*_splitsX + i,
					_nSplits,
					i,j,k,
					_splitsX, _splitsY, _splitsZ,
					_domainWidth, _domainHeight, _domainLength,
					_domainWidth /_splitsX + (i==_splitsX-1 ? _domainWidth %_splitsX : 0u),
					_domainHeight/_splitsY + (j==_splitsY-1 ? _domainHeight%_splitsY : 0u),
					_domainLength/_splitsZ + (k==_splitsZ-1 ? _domainLength%_splitsZ : 0u),
					_extraBorderSize,
					initialCondition_
				);

				_subDomains.push_back(dom);
			}
		}
	}

	assert(_subDomains.size() == _nSplits);
}

template <typename T, unsigned int N>
void MultiBufferedDomain<T,N>::linkSubDomains() {
	unsigned int i,j,k;
	for(MultiBufferedSubDomain<T,N> *dom : _subDomains) {
		i = dom->idx();
		j = dom->idy();
		k = dom->idz();
		dom->setExternalEdges(
			(i==0 ? 0 : (*this)(i-1, j  , k  )->internalEdgesRight()),
			(j==0 ? 0 : (*this)(i  , j-1, k  )->internalEdgesDown() ),
			(k==0 ? 0 : (*this)(i  , j  , k-1)->internalEdgesBack() ),

			(i==_splitsX-1 ? 0 : (*this)(i+1, j  , k  )->internalEdgesLeft() ),
			(j==_splitsY-1 ? 0 : (*this)(i  , j+1, k  )->internalEdgesTop()  ),
			(k==_splitsZ-1 ? 0 : (*this)(i  , j  , k+1)->internalEdgesFront())
		);

	}
}

template <typename T, unsigned int N>
Grid<T>* MultiBufferedDomain<T,N>::toGrid(unsigned int buffer) {

	Grid<T> *grid;

	log_console->infoStream() << "Creating a grid from domain : " << toStringDimension(_domainWidth, _domainHeight, _domainLength);

	if(_domainLength == 1u && _domainHeight == 1u) 
		grid = new Grid1D<T>(1.0f, _domainWidth, true);
	else if(_domainLength == 1u) 
		grid = new Grid2D<T>(1.0f, 1.0f, _domainWidth, _domainHeight, true);
	else 
		grid = new Grid3D<T>(1.0f, 1.0f, 1.0f, _domainWidth, _domainHeight, _domainLength, true);
	
	T* dstData = grid->data();
	for (unsigned int k = 0; k < _splitsZ; k++ ) {
		for (unsigned int j = 0; j < _splitsY; j++) {
			for (unsigned int i = 0; i < _splitsX; i++) {
				MultiBufferedSubDomain<T,N> *subDomain = (*this)(i,j,k);
				T* srcData = *(subDomain->data() + buffer);
				for (unsigned int kk = 0; kk < subDomain->subDomainLength(); kk++) {
					for (unsigned int jj = 0; jj < subDomain->subDomainHeight(); jj++) {
						unsigned long dstOffset = 0ul
							+ subDomain->offset() 
							+ kk*_domainHeight*_domainWidth 
							+ jj*_domainWidth;
						unsigned long srcOffset = 0ul
							+ kk*subDomain->subDomainHeight()*subDomain->subDomainWidth()
							+ jj*subDomain->subDomainWidth();
						memcpy(dstData + dstOffset, srcData + srcOffset, subDomain->subDomainWidth()*sizeof(T));
					}
				}
			}
		}
	}

	return grid;
}
		
template <typename T, unsigned int N>
T* MultiBufferedDomain<T,N>::allocateSliceX() {
	return new T[_domainHeight*_domainLength];
}

template <typename T, unsigned int N>
T* MultiBufferedDomain<T,N>::allocateSliceY() {
	return new T[_domainWidth*_domainLength];
}

template <typename T, unsigned int N>
T* MultiBufferedDomain<T,N>::allocateSliceZ() {
	return new T[_domainWidth*_domainHeight];
}
