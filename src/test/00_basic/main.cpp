#include <UThreadPool/ThreadPool.hpp>

#include <iostream>

using namespace Ubpa;

int main() {
	// create thread pool with 4 worker threads
	ThreadPool pool(4);

	// enqueue and store future
	auto result = pool.ReturnEnqueue([](int answer) { return answer; }, 42);

	// get result from future
	std::cout << result.get() << std::endl;

	return 0;
}
