// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/concurrent_queue.h>
#include <lib/exception.h>
#include <lib/handle_allocator.h>

namespace ionengine {

struct Job {
    std::function<void()> func;
    uint16_t id;
    uint8_t finished;
};

class ThreadPool {
public:

    enum { MaxJobs = 256 };

    ThreadPool(uint32_t const thread_count);
    
    template<class Func, class... Args>
    Handle<Job> push(Func&& func, Args&&... args) {

        /*auto it = _workers.begin();
        std::advance(it, thread_current);

        uint32_t job_id = (static_cast<uint32_t>(thread_current) << 16) + it->current_job;
        
        bool result = it->active_jobs.try_push(
            Job {
                std::bind(std::forward<Func>(func), std::forward<Args>(args)...),
                it->current_job,
                0x0
            }
        );

        if(!result) {
            throw Exception(u8"Unable to push task to thread pool");
        }

        it->current_job = (it->current_job + 1) % MaxActiveJobs;
        thread_current = (thread_current + 1) % static_cast<uint16_t>(_workers.size());
        ++_active_job_count;

        _cond_jobs.notify_one();
        return JobHandle { job_id };*/

        return INVALID_HANDLE(Job);
    }

    bool is_finished(Handle<Job> const& job);

    void wait(Handle<Job> const& job);

    void wait_all();

    void join();

private:

    struct Worker {
        std::thread thread;
        uint16_t current_job;
        ConcurrentQueue<Job, MaxJobs> active_jobs;
        std::array<Job, MaxJobs> finished_jobs;
    };

    HandleAllocator<Job> _job_allocator;

    std::mutex _mutex;
    std::condition_variable _cond_jobs;

    std::atomic<uint16_t> _active_job_count;
    bool _exec{true};
    uint16_t thread_current{0};
    std::list<Worker> _workers;
};

}