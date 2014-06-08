
#ifndef SUBDOMAIN_H
#define SUBDOMAIN_H

#include "initialCond.hpp"


template <typename T, unsigned int N = 2u>
class MultiBufferedSubDomain {

	public:
		MultiBufferedSubDomain(unsigned int id, unsigned int nSubDomain,
				unsigned int idx, unsigned int idy, unsigned int idz, 
				unsigned int nSubDomainX, unsigned int nSubDomainY, unsigned int nSubDomainZ,
				unsigned int domainWidth, unsigned int domainHeight, unsigned int domainLength,
				unsigned int subDomainWidth, unsigned int subDomainHeight, unsigned int subDomainLength,
				unsigned int borderSize,
				InitialCond<T> *initialCondition);

		~MultiBufferedSubDomain();

		unsigned int id() const;
		unsigned int nSubDomain() const;	

		unsigned int idx() const;
		unsigned int idy() const;
		unsigned int idz() const;

		unsigned int nSubDomainX() const;		
		unsigned int nSubDomainY() const;		
		unsigned int nSubDomainZ() const;

		unsigned int domainWidth() const;		
		unsigned int domainHeight() const;		
		unsigned int domainLength() const;

		unsigned int subDomainWidth() const;		
		unsigned int subDomainHeight() const;		
		unsigned int subDomainLength() const;

		unsigned int subDomainBaseWidth() const;		
		unsigned int subDomainBaseHeight() const;		
		unsigned int subDomainBaseLength() const;

		unsigned int borderSize() const;

		T* const* data() const;
		unsigned long size() const;
		unsigned long bytes() const;

		T* const* internalEdgesLeft() const;
		T* const* internalEdgesRight() const;
		T* const* internalEdgesTop() const;
		T* const* internalEdgesDown() const;
		T* const* internalEdgesFront() const;
		T* const* internalEdgesBack() const;
		T* const** internalEdges() const;

		T** externalEdgesLeft() const;
		T** externalEdgesRight() const;
		T** externalEdgesTop() const;
		T** externalEdgesDown() const;
		T** externalEdgesFront() const;
		T** externalEdgesBack() const;
		T*** externalEdges() const;

		unsigned long edgeSizeX() const;
		unsigned long edgeSizeY() const;
		unsigned long edgeSizeZ() const;

		unsigned long edgeBytesX() const;
		unsigned long edgeBytesY() const;
		unsigned long edgeBytesZ() const;

		unsigned int offsetX() const;
		unsigned int offsetY() const;
		unsigned int offsetZ() const;

		unsigned long offset() const;

		void setExternalEdges(T* const* edgeLeft, T* const* edgeTop, T* const* edgeFront,
				T* const* edgeRight, T* const* edgeDown, T* const* edgeBack); 

		void initSubDomain(unsigned int bufferId);

	private:
		const unsigned int _id;
		const unsigned int _nSubDomain;
		const unsigned int _idx, _idy, _idz;
		const unsigned int _nSubDomainX,  _nSubDomainY,  _nSubDomainZ;
		const unsigned int _domainWidth,  _domainHeight,  _domainLength;
		const unsigned int _subDomainWidth,  _subDomainHeight,  _subDomainLength;
		const unsigned int _subDomainBaseWidth,  _subDomainBaseHeight,  _subDomainBaseLength;
		const unsigned int _borderSize;

		const unsigned long _size, _bytes;
		const unsigned long _edgeSizeX, _edgeSizeY, _edgeSizeZ;
		const unsigned long _edgeBytesX, _edgeBytesY, _edgeBytesZ;

		const InitialCond<T> *_initialCondition;

		T *_data[N];

		T *_internalEdgesLeft[N], *_internalEdgesRight[N];
		T *_internalEdgesTop[N], *_internalEdgesDown[N];
		T *_internalEdgesFront[N], *_internalEdgesBack[N];

		T **_externalEdgesLeft, **_externalEdgesRight;
		T **_externalEdgesTop, **_externalEdgesDown;
		T **_externalEdgesFront, **_externalEdgesBack;

};

#include "subDomain.tpp"

#endif /* end of include guard: SUBPROBLEM_H */
