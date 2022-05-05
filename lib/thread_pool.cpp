// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <lib/thread_pool.h>

using namespace ionengine;
using namespace ionengine::lib;

ThreadPool::ThreadPool(uint32_t const thread_count) : 
    _worker_count(thread_count),
    _workers(std::make_unique<Worker[]>(thread_count)), 
    _is_exec(true) {

    for(uint32_t i = 0; i < thread_count; ++i) {
        _workers[i].thread = std::thread(
            [&](Worker& worker) {
                while(_is_exec) {
                    if(worker.tasks.empty()) {
                        std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
                    } else {
                        std::packaged_task<void()> task;
                        {
                            task = std::move(worker.tasks.front());
                            worker.tasks.pop();
                        }
                        task();
                        --_pending_tasks_count;
                    }
                }
            },
            std::ref(_workers[i])
        );
    }
}

size_t ThreadPool::size() const { 
    return static_cast<size_t>(_worker_count); 
}

void ThreadPool::wait_all() const {
    assert(_is_exec && "An error occurred while waiting for the completion of tasks in the thread pool");

    while(_pending_tasks_count.load() > 0) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
    }
}

void ThreadPool::join() {
    _is_exec = false;
    
    for(size_t i = 0; i < size(); ++i) {
        _workers[i].thread.join();
    }
}
