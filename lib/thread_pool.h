// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/concurrent_queue.h>
#include <lib/handle.h>
#include <lib/exception.h>

namespace ionengine {

struct Job {
    uint16_t id;
    uint8_t gen;
    std::function<void()> func;
    bool is_finished;
};

class ThreadPool {
public:

    enum { 
        MaxJobsPerWorker = 256
    };

    ThreadPool(uint32_t const thread_count);

    ThreadPool(ThreadPool const&) = delete;

    ThreadPool(ThreadPool&&) noexcept = delete;

    ThreadPool& operator=(ThreadPool const&) = delete;

    ThreadPool& operator=(ThreadPool&&) noexcept = delete;
    
    template<class Func, class... Args>
    Handle<Job> push(Func&& func, Args&&... args) {

        uint32_t const job_id = // Job ID = | uint32_t ( uint16_t ( Thread Index | Generation Index ) | uint16_t Pending Job Index)
            static_cast<uint32_t>(static_cast<uint16_t>(_current_thread) << 8 | static_cast<uint16_t>(_current_generation)) << 16 +
            static_cast<uint32_t>(_last_pending_jobs[_current_thread]);
        
        auto pending_job = Job {};
        pending_job.id = _last_pending_jobs[_current_thread];
        pending_job.gen = _current_generation;
        pending_job.func = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

        bool result = _workers[_current_thread].pending_jobs.try_push(std::move(pending_job));

        if(!result) {
            throw Exception(u8"An error occurred while adding a new task to the thread pool");
        }

        _last_pending_jobs[_current_thread] = (_last_pending_jobs[_current_thread] + 1) % MaxJobsPerWorker;
        
        _current_generation = (_current_generation + 1) % 255;
        _current_thread = (_current_thread + 1) % static_cast<uint8_t>(_workers.size());
        
        ++_pending_jobs_count;

        _wait_jobs.notify_one();

        return Handle<Job>(job_id);
    }

    bool is_finished(Handle<Job> const& job) const;

    void wait(Handle<Job> const& job) const;

    void wait_all() const;

    void join();

private:

    struct Worker {
        std::thread thread;
        ConcurrentQueue<Job, MaxJobsPerWorker> pending_jobs;
        std::array<Job, MaxJobsPerWorker> finished_jobs;
    };

    bool _exec{true};

    uint8_t _current_thread{0};
    uint8_t _current_generation{0};

    std::atomic<uint16_t> _pending_jobs_count;
    std::vector<uint16_t> _last_pending_jobs;

    std::mutex _mutex;
    std::condition_variable _wait_jobs;

    std::vector<Worker> _workers;
};

}
