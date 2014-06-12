
#include "utils.hpp"

#include "grid3D.hpp"
#include "grid2D.hpp"
#include "grid1D.hpp"

using namespace log4cpp;
using namespace utils;

template <typename T, unsigned int N>
MultiBufferedSubDomain<T,N>::MultiBufferedSubDomain(unsigned int id_, unsigned int nSubDomain_,
		unsigned int idx_, unsigned int idy_, unsigned int idz_, 
		unsigned int nSubDomainX_, unsigned int nSubDomainY_, unsigned int nSubDomainZ_,
		unsigned int domainWidth_, unsigned int domainHeight_, unsigned int domainLength_,
		unsigned int subDomainWidth_, unsigned int subDomainHeight_, unsigned int subDomainLength_,
		unsigned int borderSize_,
		InitialCond<T> *initialCondition_) :
	_id(id_),
	_nSubDomain(nSubDomain_),
	_idx(idx_), _idy(idy_), _idz(idz_),
	_nSubDomainX(nSubDomainX_), _nSubDomainY(nSubDomainY_), _nSubDomainZ(nSubDomainZ_),
	_domainWidth(domainWidth_), _domainHeight(domainHeight_), _domainLength(domainLength_),
	_subDomainWidth(subDomainWidth_), _subDomainHeight(subDomainHeight_), _subDomainLength(subDomainLength_),
	_subDomainBaseWidth(_domainWidth/_nSubDomainX), 
	_subDomainBaseHeight(_domainHeight/_nSubDomainY), 
	_subDomainBaseLength(_domainLength/_nSubDomainZ), 
	_borderSize(borderSize_),
	_size(_subDomainWidth*_subDomainHeight*_subDomainLength), 
	_bytes(_size*sizeof(T)),
	_edgeSizeX(_subDomainHeight*_subDomainLength*_borderSize),
	_edgeSizeY(_subDomainWidth*_subDomainLength*_borderSize),
	_edgeSizeZ(_subDomainWidth*_subDomainHeight*_borderSize),
	_edgeBytesX(_edgeSizeX*sizeof(T)), 
	_edgeBytesY(_edgeSizeY*sizeof(T)), 
	_edgeBytesZ(_edgeSizeZ*sizeof(T)),
	_initialCondition(initialCondition_)
{

	for (unsigned int i = 0; i < N; i++) {
		_data[i] = new T[_size];

		_internalEdgesLeft[i] = new T[_edgeSizeX];
		_internalEdgesRight[i] = new T[_edgeSizeX];

		_internalEdgesTop[i] = new T[_edgeSizeY];
		_internalEdgesDown[i] = new T[_edgeSizeY];

		_internalEdgesFront[i] = new T[_edgeSizeZ];
		_internalEdgesBack[i] = new T[_edgeSizeZ];
	}


	log_console->debugStream() 
		<< "Created a " << toStringDimension(_subDomainWidth, _subDomainHeight, _subDomainLength) << " multibuffered subDomain"
		<< " (nBuffer=" << N
		<< " ,base=" << toStringDimension(_subDomainBaseWidth, _subDomainBaseHeight, _subDomainBaseLength)
		<< ", global=" << toStringDimension(_domainWidth, _domainHeight, _domainLength) << ")"
		<< ", id=" << _id << "/" << _nSubDomain-1u << "  [ "
		<< toStringVec3(_idx,_idy,_idz) << " // " << toStringVec3(_nSubDomainX-1u, _nSubDomainY-1u, _nSubDomainZ-1u)
		<< " ]" 
		<< ", borderSize=" << _borderSize 
		<< ", storage=" << N << " x " << toStringMemory(_bytes) << " = " << toStringMemory(N*_bytes)<< ".";
}

template <typename T, unsigned int N>
MultiBufferedSubDomain<T,N>::~MultiBufferedSubDomain() {
	T **arrays[13] = {
		_data,
		_internalEdgesLeft, _internalEdgesRight,
		_internalEdgesTop, _internalEdgesDown,
		_internalEdgesFront, _internalEdgesBack,
	};

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < N; j++) {
			delete[] arrays[i][j];
		}
	}
}

template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::id() const {
	return _id;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::idx() const {
	return _idx;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::idy() const {
	return _idy;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::idz() const {
	return _idz;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::nSubDomain() const {
	return _nSubDomain;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::nSubDomainX() const {
	return _nSubDomainX;
}		
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::nSubDomainY() const {
	return _nSubDomainY;
}		
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::nSubDomainZ() const {
	return _nSubDomainZ;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::domainWidth() const {
	return _domainWidth;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::domainHeight() const {
	return _domainHeight;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::domainLength() const {
	return _domainLength;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::subDomainWidth() const {
	return _subDomainWidth;
}		
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::subDomainHeight() const {
	return _subDomainHeight;
}		
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::subDomainLength() const {
	return _subDomainLength;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::subDomainBaseWidth() const {
	return _subDomainBaseWidth;
}		
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::subDomainBaseHeight() const {
	return _subDomainBaseHeight;
}		
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::subDomainBaseLength() const {
	return _subDomainBaseLength;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::borderSize() const {
	return _borderSize;
}
template <typename T, unsigned int N>
T* const* MultiBufferedSubDomain<T,N>::data() const {
	return _data;
}
template <typename T, unsigned int N>
unsigned long MultiBufferedSubDomain<T,N>::size() const {
	return _size;
}
template <typename T, unsigned int N>
unsigned long MultiBufferedSubDomain<T,N>::bytes() const {
	return _bytes;
}
template <typename T, unsigned int N>
T* const* MultiBufferedSubDomain<T,N>::internalEdgesLeft() const {
	return _internalEdgesLeft;
}
template <typename T, unsigned int N>
T* const* MultiBufferedSubDomain<T,N>::internalEdgesRight() const {
	return _internalEdgesRight;
}
template <typename T, unsigned int N>
T* const* MultiBufferedSubDomain<T,N>::internalEdgesTop() const {
	return _internalEdgesTop;
}
template <typename T, unsigned int N>
T* const* MultiBufferedSubDomain<T,N>::internalEdgesDown() const {
	return _internalEdgesDown;
}
template <typename T, unsigned int N>
T* const* MultiBufferedSubDomain<T,N>::internalEdgesFront() const {
	return _internalEdgesFront;
}
template <typename T, unsigned int N>
T* const* MultiBufferedSubDomain<T,N>::internalEdgesBack() const {
	return _internalEdgesBack;
}
template <typename T, unsigned int N>
T** MultiBufferedSubDomain<T,N>::externalEdgesLeft() const {
	return _externalEdgesLeft;
}
template <typename T, unsigned int N>
T** MultiBufferedSubDomain<T,N>::externalEdgesRight() const {
	return _externalEdgesRight;
}
template <typename T, unsigned int N>
T** MultiBufferedSubDomain<T,N>::externalEdgesTop() const {
	return _externalEdgesTop;
}
template <typename T, unsigned int N>
T** MultiBufferedSubDomain<T,N>::externalEdgesDown() const {
	return _externalEdgesDown;
}
template <typename T, unsigned int N>
T** MultiBufferedSubDomain<T,N>::externalEdgesFront() const {
	return _externalEdgesFront;
}
template <typename T, unsigned int N>
T** MultiBufferedSubDomain<T,N>::externalEdgesBack() const {
	return _externalEdgesBack;
}
template <typename T, unsigned int N>
unsigned long MultiBufferedSubDomain<T,N>::edgeSizeX() const {
	return _edgeSizeX;
}
template <typename T, unsigned int N>
unsigned long MultiBufferedSubDomain<T,N>::edgeSizeY() const {
	return _edgeSizeY;
}
template <typename T, unsigned int N>
unsigned long MultiBufferedSubDomain<T,N>::edgeSizeZ() const {
	return _edgeSizeZ;
}
template <typename T, unsigned int N>
unsigned long MultiBufferedSubDomain<T,N>::edgeBytesX() const {
	return _edgeBytesX;
}
template <typename T, unsigned int N>
unsigned long MultiBufferedSubDomain<T,N>::edgeBytesY() const {
	return _edgeBytesY;
}
template <typename T, unsigned int N>
unsigned long MultiBufferedSubDomain<T,N>::edgeBytesZ() const {
	return _edgeBytesZ;
}


template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::offsetX() const {
	return _idx*_subDomainBaseWidth;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::offsetY() const {
	return _idy*_subDomainBaseHeight;
}
template <typename T, unsigned int N>
unsigned int MultiBufferedSubDomain<T,N>::offsetZ() const {
	return _idz*_subDomainBaseLength;
}
template <typename T, unsigned int N>
unsigned long MultiBufferedSubDomain<T,N>::offset() const {
	return 0ul
		+ offsetZ() * _domainHeight * _domainWidth
		+ offsetY() * _domainWidth 
		+ offsetX();
}


template <typename T, unsigned int N>
void MultiBufferedSubDomain<T,N>::setExternalEdges(
		T* const* edgeLeft_ , T* const* edgeTop_ , T* const* edgeFront_,
		T* const* edgeRight_, T* const* edgeDown_, T* const* edgeBack_ ) { 

	_externalEdgesLeft = (T**)edgeLeft_;
	_externalEdgesRight = (T**)edgeRight_;

	_externalEdgesTop = (T**)edgeTop_;
	_externalEdgesDown = (T**)edgeDown_;

	_externalEdgesFront = (T**)edgeFront_;
	_externalEdgesBack = (T**)edgeBack_;
}


template <typename T, unsigned int N>
void MultiBufferedSubDomain<T,N>::initSubDomain(unsigned int bufferId) {

	//coeur de la grille
	_initialCondition->initializeSubGrid(_data[bufferId], 
			offsetX(), offsetY(), offsetZ(), 
			_subDomainWidth, _subDomainHeight, _subDomainLength,
			_domainWidth, _domainHeight, _domainLength);

	//6 bords internes
	_initialCondition->initializeSubGrid(_internalEdgesLeft[bufferId],
			0, 0, 0, 
			_borderSize, _subDomainHeight, _subDomainLength,
			_domainWidth, _domainHeight, _domainLength);
	
	_initialCondition->initializeSubGrid(_internalEdgesRight[bufferId],
			_subDomainWidth-_borderSize, 0, 0, 
			_borderSize, _subDomainHeight, _subDomainLength,
			_domainWidth, _domainHeight, _domainLength);
	
	_initialCondition->initializeSubGrid(_internalEdgesTop[bufferId],
			0, 0, 0, 
			_subDomainWidth, _borderSize, _subDomainLength,
			_domainWidth, _domainHeight, _domainLength);
	
	_initialCondition->initializeSubGrid(_internalEdgesDown[bufferId],
			0, _subDomainHeight-_borderSize, 0, 
			_subDomainWidth, _borderSize, _subDomainLength,
			_domainWidth, _domainHeight, _domainLength);
	
	_initialCondition->initializeSubGrid(_internalEdgesFront[bufferId],
			0, 0, 0, 
			_subDomainWidth, _subDomainHeight, _borderSize,
			_domainWidth, _domainHeight, _domainLength);
	
	_initialCondition->initializeSubGrid(_internalEdgesBack[bufferId],
			0, 0, _subDomainLength-_borderSize, 
			_subDomainWidth, _subDomainHeight, _borderSize,
			_domainWidth, _domainHeight, _domainLength);
	
}


template <typename T, unsigned int N>
T* const** MultiBufferedSubDomain<T,N>::internalEdges() const {

	T* const** edges = new T* const*[6];
	edges[0] = _internalEdgesLeft;
	edges[1] = _internalEdgesRight;
	edges[2] = _internalEdgesTop;
	edges[3] = _internalEdgesDown;
	edges[4] = _internalEdgesFront;
	edges[5] = _internalEdgesBack;

	return edges;
}

template <typename T, unsigned int N>
T*** MultiBufferedSubDomain<T,N>::externalEdges() const {

	T*** edges = new T**[6];
	edges[0] = _externalEdgesLeft;
	edges[1] = _externalEdgesRight;
	edges[2] = _externalEdgesTop;
	edges[3] = _externalEdgesDown;
	edges[4] = _externalEdgesFront;
	edges[5] = _externalEdgesBack;

	return edges;
}
