
#ifndef GRID_HPP
#define GRID_HPP

#include <iostream>

template <typename T>
class Grid {

	public:
		virtual ~Grid();

		T realWidth() const;
		T realHeight() const;
		T realLength() const;

		unsigned int width() const;
		unsigned int height() const;
		unsigned int length() const;

		virtual unsigned long size() const = 0;
		virtual unsigned long bytes() const = 0;
		
		T dh() const;
		unsigned int dim() const;
		bool isAllocated() const;
		
		T operator[](unsigned int n) const;
		T& operator[](unsigned int n);
		
		T operator()(unsigned int i, unsigned int j, unsigned int k) const; 
		T& operator()(unsigned int i, unsigned int j, unsigned int k); 
		
		T *data() const;

		virtual void allocateOnCpu() = 0;
		void freeOnCpu();
	
		//ne copie pas le pointeur, il reste à NULL
		virtual Grid<T> *clone() const = 0;

		void save(const std::string &dst);		 //serialisation de la classe
		void exportData(const std::string &dst); //format lisible (pour gnuplot)

	protected:
		explicit Grid();
		
		//Ne copie pas les données, laisse le pointeur à NULL
		Grid(const Grid &grid);
		
		explicit Grid(T realWidth_, T realHeight_, T realLength_,
				unsigned int width_, unsigned int height_, unsigned int length_,
				unsigned int dim_, bool allocate = true);
		
		explicit Grid(T realWidth_, T realHeight_, T realLength_,
				T dh_,
				unsigned int dim_, bool allocate = true);
		
		explicit Grid(unsigned int width_, unsigned int height_, unsigned int length_,
				T dh_,
				unsigned int dim_, bool allocate = true);

		T _realWidth, _realHeight, _realLength;
		unsigned int _width,_height,_length;

		T _dh;

		unsigned int _dim;
		bool _isAllocated;

		T *_data;
};

template <typename T>
std::ostream & operator <<(std::ostream &out, const Grid<T> &grid);

#include "grid.tpp"

#endif
