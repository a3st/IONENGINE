
#include <precompiled.h>
#include <lib/thread_pool.h>

using namespace ionengine;

ThreadPool::ThreadPool(uint32_t const thread_count) {

    _workers.resize(thread_count);
    _last_pending_jobs.resize(thread_count);

    for(uint32_t i = 0; i < thread_count; ++i) {

        _workers[i].thread = std::thread(
            [&](Worker& worker) {

                while(_exec) {
                    if(worker.pending_jobs.empty()) {

                        std::unique_lock lock(_mutex);
                        _wait_jobs.wait(lock, [&]() -> bool { return !_exec || !worker.pending_jobs.empty(); });

                    } else {

                        auto job = Job {};

                        if(worker.pending_jobs.try_pop(job)) {

                            job.func();
                            job.is_finished = true;

                            worker.finished_jobs[job.id] = job;
                            
                            --_pending_jobs_count;
                        }
                    }
                }
            },
            std::ref(_workers[i])
        );
    }
}

bool ThreadPool::is_finished(Handle<Job> const& job) const {
    
    uint16_t const hiword = (job.id() >> 16) & 0xffff;
    uint16_t const loword = job.id() & 0xffff;

    uint8_t const thread_index = (job.id() >> 8) & 0xff;
    uint8_t const generation_index = job.id() & 0xff;

    return _workers[thread_index].finished_jobs[loword].is_finished && _workers[thread_index].finished_jobs[loword].gen == generation_index;
}

void ThreadPool::wait(Handle<Job> const& job) const {

    if(!_exec) {
        throw Exception(u8"An error occurred while waiting for the completion of task in the thread pool");
    }

    uint16_t const hiword = (job.id() >> 16) & 0xffff;
    uint16_t const loword = job.id() & 0xffff;

    uint8_t const thread_index = (job.id() >> 8) & 0xff;
    uint8_t const generation_index = job.id() & 0xff;

    while(_workers[thread_index].finished_jobs[loword].is_finished && _workers[thread_index].finished_jobs[loword].gen == generation_index) {

        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }
}

void ThreadPool::wait_all() const {

    if(!_exec) {
        throw Exception(u8"An error occurred while waiting for the completion of tasks in the thread pool");
    }

    while(_pending_jobs_count > 0) {

        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }
}

void ThreadPool::join() {

    _exec = false;

    _wait_jobs.notify_all();
    
    for(auto& worker : _workers) {

        worker.thread.join();
    }
}
