#pragma once

namespace Ubpa {
    template<class F, class... Args>
    auto ThreadPool::Enqueue(F&& f, Args&&... args) {
        using return_type = std::invoke_result_t<F, Args...>;

        std::promise<return_type> barrier;

        std::future<return_type> rst = barrier.get_future();
        {
            std::unique_lock<std::mutex> lock(mutex_tasks);

            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([f = std::forward<F>(f), barrier = std::move(barrier), ... args = std::forward<Args>(args)]() mutable {
                if constexpr (std::is_void_v<return_type>) {
                    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
                    barrier.set_value();
                }
                else
                    barrier.set_value(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
            });
        }
        condition.notify_one();
        return rst;
    }
}
