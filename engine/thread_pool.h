// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/lockfree_queue.h>
#include <engine/exception.h>

namespace ionengine {

class JobHandle {
public:

    JobHandle() = default;
    JobHandle(uint32_t const id) : _id(id) { }

private:
    
    friend class ThreadPool;
    uint32_t _id;
};

class ThreadPool {
public:

    ThreadPool(uint16_t const thread_count);
    
    template<class Func, class... Args>
    JobHandle push(Func&& func, Args&&... args) {

        auto it = _workers.begin();
        std::advance(it, thread_current);

        uint32_t job_id = (static_cast<uint32_t>(thread_current) << 16) + it->current_job;
        //std::cout << std::format("generate job handle id: {:#06x}", job_id) << std::endl;
        
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
        return JobHandle { job_id };
    }

    bool is_finished(JobHandle const& handle);

    void wait(JobHandle const& handle);

    void wait_all();

    void join();

private:

    enum { MaxActiveJobs = 256 };

    struct Job {
        std::function<void()> func;
        uint16_t id;
        uint8_t finished;
    };

    struct Worker {
        std::thread thread;
        uint16_t current_job;
        ConcurrentQueue<Job, MaxActiveJobs> active_jobs;
        std::array<Job, MaxActiveJobs> finished_jobs;
    };

    std::mutex _mutex;
    std::condition_variable _cond_jobs;

    std::atomic<uint16_t> _active_job_count;
    bool _exec{true};
    uint16_t thread_current{0};
    std::list<Worker> _workers;
};

}