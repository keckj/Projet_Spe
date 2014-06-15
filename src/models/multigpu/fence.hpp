
#ifndef FENCE_H
#define FENCE_H

#include <mutex>
#include <condition_variable>
#include <boost/thread/barrier.hpp>

class Fence {
public:
	Fence(unsigned int N);
	~Fence();

	void operator()();

private:
	boost::barrier barrier;
};

	
#endif /* end of include guard: FENCE_H */
