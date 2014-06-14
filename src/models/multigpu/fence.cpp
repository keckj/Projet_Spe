
#include "fence.hpp"
#include <thread>

Fence::Fence(unsigned int N):
	barrier(N)
{
}

Fence::~Fence() {
}

void Fence::operator()() {
	barrier.wait();
}

