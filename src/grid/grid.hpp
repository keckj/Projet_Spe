
#ifndef GRID_HPP
#define GRID_HPP

#include <iostream>

template <typename T>
class Grid {

	public:
		explicit Grid(const std::string &src);
		explicit Grid(const Grid &grid, bool copyPointer = true);
		virtual ~Grid();

		T realWidth() const;
		T realHeight() const;
		T realLength() const;

		unsigned int width() const;
		unsigned int height() const;
		unsigned int length() const;

		unsigned long size() const;
		unsigned long bytes() const;
		
		T dh() const;
		unsigned int dim() const;
		bool isAllocated() const;
		bool isOwner() const;
		
		T operator[](unsigned long n) const;
		T& operator[](unsigned long n);
		
		T operator()(unsigned int i, unsigned int j, unsigned int k) const; 
		T& operator()(unsigned int i, unsigned int j, unsigned int k); 
		
		T *data() const;


		void setData(T* data_, bool isOwner_);
		void allocate();
		void free();
	
		Grid<T> *clone() const; //copie légère de la matrice
		Grid<T> *cloneAttribs() const; //copie légère de la matrice, pointeur de donnée mis à NULL
		Grid<T> *cloneData() const; //copie profonde de la matrice

		void exportGrid(const std::string &dst); //serialisation de la classe
		void exportData(const std::string &dst); //format lisible (pour gnuplot)

	protected:
		explicit Grid();
		explicit Grid(T realWidth_, T realHeight_, T realLength_,
				unsigned int width_, unsigned int height_, unsigned int length_,
				unsigned int dim_);
		
		explicit Grid(T realWidth_, T realHeight_, T realLength_,
				T dh_, unsigned int dim_);
		
		explicit Grid(unsigned int width_, unsigned int height_, unsigned int length_,
				T dh_, unsigned int dim_);

		T _realWidth, _realHeight, _realLength;
		unsigned int _width,_height,_length;

		T _dh;

		unsigned int _dim;
		bool _isAllocated, _isOwner;

		T *_data;
};

template <typename T>
std::ostream & operator <<(std::ostream &out, const Grid<T> &grid);

#include "grid.tpp"

#endif
