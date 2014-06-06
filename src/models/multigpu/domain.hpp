
#ifndef DOMAIN_H
#define DOMAIN_H

#include <vector>

#include "subDomain.hpp"
#include "initialCond.hpp"

template <typename T, unsigned int N>
class MultiBufferedDomain {

	public: 
		MultiBufferedDomain(unsigned int domainWidth, unsigned int domainHeight, unsigned int domainLength,
			unsigned int extraBorderSize, 
			unsigned int minSplits,
			InitialCond<float> *initialCondition);

		~MultiBufferedDomain();

		unsigned int domainWidth() const;
		unsigned int domainHeight() const;
		unsigned int domainLength() const;

		unsigned int extraBorderSize() const;

		unsigned int nSplits() const;
		unsigned int splitsX() const;
		unsigned int splitsY() const;
		unsigned int splitsZ() const;

		MultiBufferedSubDomain<T,N> *operator()(unsigned int i, unsigned int j, unsigned int k);
		MultiBufferedSubDomain<T,N> *operator[](unsigned int n);
		
		Grid<T>* toGrid(unsigned int buffer);

	private:
		unsigned int _domainWidth, _domainHeight, _domainLength;
		unsigned int _extraBorderSize;

		unsigned int _nSplits;
		unsigned int _splitsX, _splitsY, _splitsZ;

		std::vector<MultiBufferedSubDomain<T,N>*> _subDomains;

		void splitDomain(unsigned int minSplits, InitialCond<float> *initialCond);
		void linkSubDomains();
		
};

#include "domain.tpp"

#endif /* end of include guard: DOMAIN_H */
