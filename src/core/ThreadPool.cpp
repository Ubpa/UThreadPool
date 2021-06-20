#include <UThreadPool/ThreadPool.hpp>

using namespace Ubpa;

ThreadPool::ThreadPool(size_t num) {
    for (size_t i = 0; i < num; ++i) {
        workers.emplace_back([this] {
            while (true) {
                unique_function<void()> task;
                { // get task
                    std::unique_lock<std::mutex> lock(mutex_tasks);
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });
                    if (stop && tasks.empty())
                        return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mutex_tasks);
        stop = true;
    }
    condition.notify_all();
    for (auto& worker : workers)
        worker.join();
}
