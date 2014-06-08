
#ifndef FENCE_H
#define FENCE_H

#include <mutex>
#include <condition_variable>

class Fence {
public:
	Fence(unsigned int N);
	~Fence();

	void operator()();

private:
	std::mutex _mutex;
	std::condition_variable _cond;

	unsigned int _N;
	unsigned int _counter, _passed;
};

	
#endif /* end of include guard: FENCE_H */
