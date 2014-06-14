
#include "fence.hpp"
#include <thread>

Fence::Fence(unsigned int N_) :
_N(N_), _counter(0), _passed(0)
{
}

Fence::~Fence() {
}

void Fence::operator()() {
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_counter++;
		_cond.notify_one();
	}
	
	while(_counter < _N) std::this_thread::yield();
	//_cond.wait(lock, [this]{return _counter == _N;});

	{
		std::unique_lock<std::mutex> lock(_mutex);
		_passed++;

		if(_passed == _N) {
			_counter = 0;
			_passed = 0;
		}
		_cond.notify_one();
	}

}

