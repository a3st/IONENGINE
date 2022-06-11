// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <lib/thread_pool.h>

using namespace ionengine;
using namespace ionengine::lib;

ThreadPool::Worker::Worker() : is_alive(true) {
    thread = std::thread(&ThreadPool::Worker::worker_loop, this);
}
        
ThreadPool::Worker::~Worker() {
    if(thread.joinable()) {
        is_alive = false;
        available_tasks.notify_one();
        thread.join();
    }
}

void ThreadPool::Worker::worker_loop() {
    while(true) {
        uint8_t priority;
        std::function<void()> task;
        {
            std::unique_lock lock(mutex);
            available_tasks.wait(
                lock, 
                [&] { 
                    return !tasks.at(0).empty() || !tasks.at(1).empty() || !is_alive; 
                }
            );

            if(!is_alive) {
                break;
            }

            priority = !tasks.at(0).empty() ? 0 : 1; 
            task = std::move(tasks.at(priority).front());
        }

        task();
                        
        std::lock_guard lock(mutex);
        tasks.at(priority).pop();
        available_tasks.notify_one();
    }
}

void ThreadPool::Worker::wait(TaskPriorityFlags const flags) {
    if(flags & TaskPriorityFlags::High) {
        std::unique_lock lock(mutex);
        available_tasks.wait(lock, [&] { return tasks.at(0).empty(); });
    } else if(flags & TaskPriorityFlags::Low) {
        std::unique_lock lock(mutex);
        available_tasks.wait(lock, [&] { return tasks.at(1).empty(); });
    }
}

ThreadPool::ThreadPool(uint16_t const thread_count) {
    for(uint16_t i = 0; i < thread_count; ++i) {
        _workers.emplace_back(std::make_unique<ThreadPool::Worker>());
    }
}

size_t ThreadPool::size() const { 
    return _workers.size();
}

void ThreadPool::wait_all(TaskPriorityFlags const flags) {
    for(auto& worker : _workers) {
        worker->wait(flags);
    }
}
