#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <UFunction.hpp>

namespace Ubpa {
    class ThreadPool {
    public:
        ThreadPool(size_t num);
        ~ThreadPool();

        // return std::future
        template<class F, class... Args>
        auto ReturnEnqueue(F&& f, Args&&... args);

        template<class F, class... Args>
        void Enqueue(F&& f, Args&&... args);

        void BasicEnqueue(unique_function<void()> task);

    private:
        std::vector<std::thread> workers;
        std::queue<unique_function<void()>> tasks;

        std::mutex mutex_tasks;
        std::condition_variable condition;
        bool stop{ false };
    };
}

#include "details/ThreadPool.inl"
