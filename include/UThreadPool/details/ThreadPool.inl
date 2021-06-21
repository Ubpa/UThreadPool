#pragma once

namespace Ubpa {
    template<class F, class... Args>
    auto ThreadPool::ReturnEnqueue(F&& f, Args&&... args) {
        using return_type = std::invoke_result_t<F, Args...>;

        std::promise<return_type> barrier;

        std::future<return_type> rst = barrier.get_future();

        BasicEnqueue([f = std::forward<F>(f), barrier = std::move(barrier), ... args = std::forward<Args>(args)]() mutable {
            if constexpr (std::is_void_v<return_type>) {
                std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
                barrier.set_value();
            }
            else
                barrier.set_value(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
        });

        return rst;
    }

    template<class F, class... Args>
    void ThreadPool::Enqueue(F&& f, Args&&... args) {
        if constexpr (sizeof...(args) == 0)
            BasicEnqueue(std::forward<F>(f));
        else{
            BasicEnqueue([f = std::forward<F>(f), ... args = std::forward<Args>(args)]() mutable {
                std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
            });
        }
    }
}
