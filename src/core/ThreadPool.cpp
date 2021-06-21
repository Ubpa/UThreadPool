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

void ThreadPool::BasicEnqueue(unique_function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mutex_tasks);

        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.push(std::move(task));
    }
    condition.notify_one();
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mutex_tasks);
        stop = true;
    }
    condition.notify_all();
    for (auto& worker : workers)
        worker.join();
}
