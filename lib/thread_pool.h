// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

class ThreadPool {
public:

    ThreadPool(uint32_t const thread_count);

    ThreadPool(ThreadPool const&) = delete;

    ThreadPool(ThreadPool&&) noexcept = delete;

    ThreadPool& operator=(ThreadPool const&) = delete;

    ThreadPool& operator=(ThreadPool&&) noexcept = delete;
    
    template<class Func, class... Args>
    std::future<void> push(Func&& func, Args&&... args) {

        std::packaged_task<void()> pending_task(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        std::future<void> future = pending_task.get_future();
        {
            std::unique_lock lock(_mutex);
            _workers[_current_worker].tasks.emplace(std::move(pending_task));
        }
        _current_worker = (_current_worker + 1) % _worker_count;
        ++_pending_tasks_count;

        return future;
    }

    size_t size() const;

    void wait_all() const;

    void join();

private:

    struct Worker {
        std::thread thread;
        std::queue<std::packaged_task<void()>> tasks;
    };

    bool _is_exec{false};

    std::mutex _mutex;

    uint32_t _worker_count{0};
    std::unique_ptr<Worker[]> _workers;

    std::atomic<uint32_t> _pending_tasks_count{0};
    uint32_t _current_worker{0};
};

}
