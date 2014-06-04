
#include "subDomain.hpp"

#include "utils.hpp"

using namespace log4cpp;
using namespace utils;

SubDomain::SubDomain(unsigned int id_, unsigned int nSubDomain_,
		unsigned int idx_, unsigned int idy_, unsigned int idz_, 
		unsigned int nSubDomainX_, unsigned int nSubDomainY_, unsigned int nSubDomainZ_,
		unsigned int subDomainWidth_, unsigned int subDomainHeight_, unsigned int subDomainLength_,
		unsigned int borderSize_) :
	_id(id_),
	_nSubDomain(nSubDomain_),
	_idx(idx_), _idy(idy_), _idz(idz_),
	_nSubDomainX(nSubDomainX_), _nSubDomainY(nSubDomainY_), _nSubDomainZ(nSubDomainZ_),
	_subDomainWidth(subDomainWidth_), _subDomainHeight(subDomainHeight_), _subDomainLength(subDomainLength_),
	_borderSize(borderSize_),
	_size(_subDomainWidth*_subDomainHeight*_subDomainLength), 
	_bytes(_size*sizeof(float)),
	_edgeSizeX(_subDomainHeight*_subDomainLength*_borderSize),
	_edgeSizeY(_subDomainWidth*_subDomainLength*_borderSize),
	_edgeSizeZ(_subDomainWidth*_subDomainHeight*_borderSize),
	_edgeBytesX(_edgeSizeX*sizeof(float)), 
	_edgeBytesY(_edgeSizeY*sizeof(float)), 
	_edgeBytesZ(_edgeSizeZ*sizeof(float)),
	_data(0),
	_internalEdgeLeft(0), _internalEdgeRight(0),
	_internalEdgeTop(0), _internalEdgeDown(0),
	_internalEdgeFront(0), _internalEdgeBack(0),
	_externalEdgeLeft(0), _externalEdgeRight(0),
	_externalEdgeTop(0), _externalEdgeDown(0),
	_externalEdgeFront(0), _externalEdgeBack(0)
{
	_data = new float[_size];

	_internalEdgeLeft = new float[_edgeSizeX];
	_internalEdgeRight = new float[_edgeSizeX];

	_internalEdgeTop = new float[_edgeSizeY];
	_internalEdgeDown = new float[_edgeSizeY];
	
	_internalEdgeFront = new float[_edgeSizeZ];
	_internalEdgeBack = new float[_edgeSizeZ];

	log_console->debugStream() 
		<< "Created a " << toStringDimension(_subDomainWidth, _subDomainHeight, _subDomainLength) << " subDomain"
		<< ", id=" << _id << "/" << _nSubDomain-1u << "  [ "
		<< toStringVec3(_idx,_idy,_idz) << " // " << toStringVec3(_nSubDomainX-1u, _nSubDomainY-1u, _nSubDomainZ-1u)
		<< " ], borderSize=" << _borderSize 
		<< ", storage=" << toStringMemory(_bytes) << ".";
}

SubDomain::~SubDomain() {
	float *arrays[13] = {
		_data,
		_internalEdgeLeft, _internalEdgeRight,
		_internalEdgeTop, _internalEdgeDown,
		_internalEdgeFront, _internalEdgeBack,
	};

	for (int i = 0; i < 13; i++) {
		delete[] arrays[i];
	}
}

unsigned int SubDomain::id() const {
	return _id;
}
unsigned int SubDomain::idx() const {
	return _idx;
}
unsigned int SubDomain::idy() const {
	return _idy;
}
unsigned int SubDomain::idz() const {
	return _idz;
}
unsigned int SubDomain::nSubDomain() const {
	return _nSubDomain;
}
unsigned int SubDomain::nSubDomainX() const {
	return _nSubDomainX;
}		
unsigned int SubDomain::nSubDomainY() const {
	return _nSubDomainY;
}		
unsigned int SubDomain::nSubDomainZ() const {
	return _nSubDomainZ;
}
unsigned int SubDomain::subDomainWidth() const {
	return _subDomainWidth;
}		
unsigned int SubDomain::subDomainHeight() const {
	return _subDomainHeight;
}		
unsigned int SubDomain::subDomainLength() const {
	return _subDomainLength;
}
unsigned int SubDomain::borderSize() const {
	return _borderSize;
}
float *SubDomain::data() const {
	return _data;
}
unsigned long SubDomain::size() const {
	return _size;
}
unsigned long SubDomain::bytes() const {
	return _bytes;
}
float *SubDomain::internalEdgeLeft() const {
	return _internalEdgeLeft;
}
float *SubDomain::internalEdgeRight() const {
	return _internalEdgeRight;
}
float *SubDomain::internalEdgeTop() const {
	return _internalEdgeTop;
}
float *SubDomain::internalEdgeDown() const {
	return _internalEdgeDown;
}
float *SubDomain::internalEdgeFront() const {
	return _internalEdgeFront;
}
float *SubDomain::internalEdgeBack() const {
	return _internalEdgeBack;
}
float *SubDomain::externalEdgeLeft() const {
	return _externalEdgeLeft;
}
float *SubDomain::externalEdgeRight() const {
	return _externalEdgeRight;
}
float *SubDomain::externalEdgeTop() const {
	return _externalEdgeTop;
}
float *SubDomain::externalEdgeDown() const {
	return _externalEdgeDown;
}
float *SubDomain::externalEdgeFront() const {
	return _externalEdgeFront;
}
float *SubDomain::externalEdgeBack() const {
	return _externalEdgeBack;
}
unsigned long SubDomain::edgeSizeX() const {
			return _edgeSizeX;
}
unsigned long SubDomain::edgeSizeY() const {
			return _edgeSizeY;
}
unsigned long SubDomain::edgeSizeZ() const {
			return _edgeSizeZ;
}
unsigned long SubDomain::edgeBytesX() const {
			return _edgeBytesX;
}
unsigned long SubDomain::edgeBytesY() const {
			return _edgeBytesY;
}
unsigned long SubDomain::edgeBytesZ() const {
			return _edgeBytesZ;
}
		
void SubDomain::setExternalEdges(float *edgeLeft_ , float *edgeTop_ , float *edgeFront_,
							     float *edgeRight_, float *edgeDown_, float *edgeBack_  ) { 
	
	_externalEdgeLeft = edgeLeft_;
	_externalEdgeRight = edgeRight_;

	_externalEdgeTop = edgeTop_;
	_externalEdgeDown = edgeDown_;

	_externalEdgeFront = edgeFront_;
	_externalEdgeBack = edgeBack_;
}

