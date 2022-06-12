// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

enum class TaskPriorityFlags : uint8_t {
    High = 1 << 0,
    Low = 1 << 1
};

DECLARE_ENUM_CLASS_BIT_FLAG(TaskPriorityFlags)

class ThreadPool {
public:

    ThreadPool();

    ThreadPool(uint16_t const thread_count);

    ThreadPool(ThreadPool const&) = delete;

    ThreadPool(ThreadPool&&) noexcept = delete;

    ThreadPool& operator=(ThreadPool const&) = delete;

    ThreadPool& operator=(ThreadPool&&) noexcept = delete;
    
    template<class Func, class... Args>
    void push(TaskPriorityFlags const priority, Func&& func, Args&&... args) {
        uint8_t const index = priority == TaskPriorityFlags::High ? 0 : 1; 

        std::function<void()> pending_task(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        {
            auto& worker = _workers.at(_current_worker);

            std::lock_guard lock(worker->mutex);
            worker->tasks.at(index).emplace(std::move(pending_task));
            worker->available_tasks.notify_one();
        }
        _current_worker = (_current_worker + 1) % static_cast<uint16_t>(_workers.size());
    }

    size_t size() const;

    void wait_all(TaskPriorityFlags const flags = TaskPriorityFlags::High | TaskPriorityFlags::Low);

    void join();

private:

    struct Worker {
        std::thread thread;
        std::array<std::queue<std::function<void()>>, 2> tasks;
        std::mutex mutex;
        std::condition_variable available_tasks;
        bool is_alive{false};

        void worker_loop();

        void wait(TaskPriorityFlags const flags);

        Worker();

        ~Worker();
    };

    std::vector<std::unique_ptr<Worker>> _workers;

    uint16_t _current_worker{0};
};

extern ThreadPool _thread_pool;

#ifndef LIB_INITIALIZE_THREAD_POOL
#define LIB_INITIALIZE_THREAD_POOL() lib::ThreadPool lib::_thread_pool;
#endif

inline ThreadPool& thread_pool() {
    return _thread_pool;
}

}
