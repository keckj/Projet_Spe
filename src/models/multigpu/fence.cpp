
#include "fence.hpp"

Fence::Fence(unsigned int N_) :
_N(N_), _counter(0), _passed(0)
{
}

Fence::~Fence() {
}

void Fence::operator()() {
	std::unique_lock<std::mutex> lock(_mutex);
	_counter++;
	_cond.wait(lock, [this]{return _counter == _N;});
	_passed++;

	if(_passed == _N) {
		_counter = 0;
		_passed = 0;
	}

	_cond.notify_one();
}

