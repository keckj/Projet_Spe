
#ifndef GRID_HPP
#define GRID_HPP

template <typename T>
class Grid {

	public:
		virtual ~Grid();

		double realWidth() const;
		double realHeight() const;
		double realLength() const;

		unsigned int width() const;
		unsigned int height() const;
		unsigned int length() const;

		virtual unsigned long size() const = 0;
		virtual unsigned long bytes() const = 0;
		
		double dh();
		unsigned int dim() const;
		bool isAllocated() const;
		
		T operator[](unsigned int n) const;
		T& operator[](unsigned int n);
		
		T operator()(unsigned int i, unsigned int j, unsigned int k) const; 
		T& operator()(unsigned int i, unsigned int j, unsigned int k); 
		
		T *data();

		virtual void allocateOnCpu() = 0;
		void freeOnCpu();


	protected:
		explicit Grid(double realWidth_, double realHeight_, double realLength_,
				unsigned int width_, unsigned int height_, unsigned int length_,
				unsigned int dim_, bool allocate = true);
		
		explicit Grid(double realWidth_, double realHeight_, double realLength_,
				double dh_,
				unsigned int dim_, bool allocate = true);
		
		explicit Grid(unsigned int width_, unsigned int height_, unsigned int length_,
				double dh_,
				unsigned int dim_, bool allocate = true);
		
		double _realWidth, _realHeight, _realLength;
		unsigned int _width,_height,_length;

		double _dh;

		unsigned int _dim;
		bool _isAllocated;

		T *_data;
};

#include "grid.tpp"

#endif
