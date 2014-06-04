
#include "domain.hpp"
#include <cassert>
		
Domain::Domain(
		unsigned int domainWidth_, unsigned int domainHeight_, unsigned int domainLength_,
		unsigned int extraBorderSize_, 
		unsigned int minSplits_) :
	_domainWidth(domainWidth_), 
	_domainHeight(domainWidth_),
	_domainLength(domainLength_),
	_extraBorderSize(extraBorderSize_)
{
	splitDomain(minSplits_);

	if(_extraBorderSize > 0u)
		linkSubDomains();
}
		
Domain::~Domain() {
	for(SubDomain* dom : _subDomains)
		delete dom;
}


unsigned int Domain::domainWidth() const {
			return _domainWidth;
}
unsigned int Domain::domainHeight() const {
			return _domainHeight;
}
unsigned int Domain::domainLength() const {
			return _domainLength;
}
unsigned int Domain::extraBorderSize() const {
			return _extraBorderSize;
}
unsigned int Domain::nSplits() const {
			return _nSplits;
}
unsigned int Domain::splitsX() const {
			return _splitsX;
}
unsigned int Domain::splitsY() const {
			return _splitsY;
}
unsigned int Domain::splitsZ() const {
			return _splitsZ;
}
		
SubDomain *Domain::operator()(unsigned int i, unsigned int j, unsigned int k) {
	return _subDomains[k*_splitsY*_splitsX + j*_splitsX + i];
}

SubDomain *Domain::operator[](unsigned int n) {
	return _subDomains[n];
}
		
void Domain::splitDomain(unsigned int minSplits) {
		
	unsigned int splitX=1, splitY=1, splitZ=1;
	unsigned int subdomainWidth=_domainWidth, subdomainHeight=_domainHeight, subdomainLength=_domainLength;
	
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

	SubDomain *dom;

	for (unsigned int k = 0; k < _splitsZ; k++) {
		for (unsigned int j = 0; j < _splitsY; j++) {
			for (unsigned int i = 0; i < _splitsX; i++) {
				dom = new SubDomain(
					k*_splitsY*_splitsX + j*_splitsX + i,
					_nSplits,
					i,j,k,
					_splitsX, _splitsY, _splitsZ,
					(i==_splitsX-1 ? _domainWidth %_splitsX : _domainWidth /_splitsX),
					(j==_splitsY-1 ? _domainHeight%_splitsY : _domainHeight/_splitsY),
					(k==_splitsZ-1 ? _domainLength%_splitsZ : _domainLength/_splitsZ),
					_extraBorderSize
				);

				_subDomains.push_back(dom);
			}
		}
	}

	assert(_subDomains.size() == _nSplits);
}

void Domain::linkSubDomains() {
	unsigned int i,j,k;
	for(SubDomain *dom : _subDomains) {
		i = dom->idx();
		j = dom->idy();
		k = dom->idz();
		dom->setExternalEdges(
			(i==0 ? 0 : (*this)(i-1, j  , k  )->internalEdgeRight()),
			(j==0 ? 0 : (*this)(i  , j-1, k  )->internalEdgeDown() ),
			(k==0 ? 0 : (*this)(i  , j  , k-1)->internalEdgeBack() ),

			(i==_splitsX-1 ? 0 : (*this)(i+1, j  , k  )->internalEdgeLeft() ),
			(j==_splitsY-1 ? 0 : (*this)(i  , j+1, k  )->internalEdgeTop()  ),
			(k==_splitsZ-1 ? 0 : (*this)(i  , j  , k+1)->internalEdgeFront())
		);

	}
}
