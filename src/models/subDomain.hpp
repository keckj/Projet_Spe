
#ifndef SUBDOMAIN_H
#define SUBDOMAIN_H


class SubDomain {
	
	public:
		SubDomain(unsigned int id, unsigned int nSubDomain,
				unsigned int idx, unsigned int idy, unsigned int idz, 
				unsigned int nSubDomainX, unsigned int nSubDomainY, unsigned int nSubDomainZ,
				unsigned int subGridWidth, unsigned int subGridHeight, unsigned int subGridLength,
				unsigned int borderSize);

		~SubDomain();

		unsigned int id() const;
		unsigned int nSubDomain() const;	

		unsigned int idx() const;
		unsigned int idy() const;
		unsigned int idz() const;

		unsigned int nSubDomainX() const;		
		unsigned int nSubDomainY() const;		
		unsigned int nSubDomainZ() const;

		unsigned int subGridWidth() const;		
		unsigned int subGridHeight() const;		
		unsigned int subGridLength() const;

		unsigned int borderSize() const;

		float *data() const;
		unsigned long size() const;
		unsigned long bytes() const;
		
		float *internalEdgeLeft() const;
		float *internalEdgeRight() const;
		float *internalEdgeTop() const;
		float *internalEdgeDown() const;
		float *internalEdgeFront() const;
		float *internalEdgeBack() const;
		
		float *externalEdgeLeft() const;
		float *externalEdgeRight() const;
		float *externalEdgeTop() const;
		float *externalEdgeDown() const;
		float *externalEdgeFront() const;
		float *externalEdgeBack() const;

		unsigned long edgeSizeX() const;
		unsigned long edgeSizeY() const;
		unsigned long edgeSizeZ() const;
		
		unsigned long edgeBytesX() const;
		unsigned long edgeBytesY() const;
		unsigned long edgeBytesZ() const;

		void setExternalEdges(float *edgeLeft , float *edgeTop , float *edgeFront,
							  float *edgeRight, float *edgeDown, float *edgeBack); 

	private:
		const unsigned int _id;
		const unsigned int _nSubDomain;
		const unsigned int _idx, _idy, _idz;
		const unsigned int _nSubDomainX,  _nSubDomainY,  _nSubDomainZ;
		const unsigned int _subGridWidth,  _subGridHeight,  _subGridLength;
		const unsigned int _borderSize;
		
		const unsigned long _size, _bytes;
		const unsigned long _edgeSizeX, _edgeSizeY, _edgeSizeZ;
		const unsigned long _edgeBytesX, _edgeBytesY, _edgeBytesZ;

		float *_data;
		
		float *_internalEdgeLeft, *_internalEdgeRight;
		float *_internalEdgeTop, *_internalEdgeDown;
		float *_internalEdgeFront, *_internalEdgeBack;
		
		float *_externalEdgeLeft, *_externalEdgeRight;
		float *_externalEdgeTop, *_externalEdgeDown;
		float *_externalEdgeFront, *_externalEdgeBack;
};



#endif /* end of include guard: SUBPROBLEM_H */
