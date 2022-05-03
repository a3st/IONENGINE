// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

struct Job {
    uint32_t id;
    uint32_t gen;
    std::function<void()> func;
};

class ThreadPool {
public:

    static uint32_t const MAX_JOBS_PER_WORKER = 256;

    ThreadPool(uint32_t const thread_count);

    ThreadPool(ThreadPool const&) = delete;

    ThreadPool(ThreadPool&&) noexcept = delete;

    ThreadPool& operator=(ThreadPool const&) = delete;

    ThreadPool& operator=(ThreadPool&&) noexcept = delete;
    
    template<class Func, class... Args>
    Handle<Job> push(Func&& func, Args&&... args) {

        uint32_t const job_id = // Job ID = | uint32_t ( uint16_t ( Thread Index | Generation Index ) | uint16_t Pending Job Index)
            static_cast<uint32_t>(static_cast<uint16_t>(_current_thread) << 8 | static_cast<uint16_t>(_current_generation)) << 16 |
            static_cast<uint32_t>(_last_pending_jobs[_current_thread]);

        auto pending_job = Job {};
        pending_job.id = _last_pending_jobs[_current_thread];
        pending_job.gen = _current_generation;
        pending_job.func = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

        bool result = _workers[_current_thread].pending_jobs.try_push(std::move(pending_job));

        assert(result && "An error occurred while adding a new task to the thread pool");

        _last_pending_jobs[_current_thread] = (_last_pending_jobs[_current_thread] + 1) % MAX_JOBS_PER_WORKER;
        
        _current_generation = (_current_generation + 1) % 255;
        _current_thread = (_current_thread + 1) % _thread_count;
        
        _pending_jobs_count.store(_pending_jobs_count.load() + 1);

        return Handle<Job>(job_id);
    }

    uint32_t size() const { return _thread_count; }

    bool is_finished(Handle<Job> const& job_data) const;

    void wait(Handle<Job> const& job_data) const;

    void wait_all() const;

    void join();

private:

    struct JobStream {
        std::atomic<uint32_t> gen;
        std::atomic<bool> is_finished;
    };

    struct Worker {
        std::thread thread;
        ConcurrentQueue<Job, MAX_JOBS_PER_WORKER> pending_jobs;
        std::array<JobStream, MAX_JOBS_PER_WORKER> finished_jobs;
    };

    bool _exec{false};

    uint32_t _current_thread{0};
    uint32_t _current_generation{0};

    std::atomic<uint32_t> _pending_jobs_count;
    std::unique_ptr<uint32_t[]> _last_pending_jobs;

    uint32_t _thread_count{0};
    std::unique_ptr<Worker[]> _workers;
};

}
