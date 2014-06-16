
#ifndef INITIALCOND_H_1234
#define INITIALCOND_H_1234

#include <thread>
#include <mutex>
#include <condition_variable>
#include "grid.hpp"

template <typename T>
class InitialCond {

	public:
		virtual ~InitialCond();

		void initializeGrid(Grid<T> *grid) const;

		void initializeSubGrid(T *data, 
								unsigned int offsetX, unsigned int offsetY, unsigned int offsetZ,
								unsigned int subGridWidth, unsigned int subGridHeight, unsigned int subGridLength,
								unsigned int gridWidth, unsigned int gridHeight, unsigned int gridLength) const;
		
		static std::mutex mutex;
		static std::condition_variable cond;
		
	protected:
		InitialCond(bool lock=false);

		// (x,y,z) in [0,1]x[0,1]x[0,1]
		virtual T F(T x, T y, T z) const = 0;

		bool _lock;
};

#include "initialCond.tpp"

#endif /* end of include guard: INITIALCOND_H */
