
#include <precompiled.h>
#include <lib/thread_pool.h>

using namespace ionengine;

ThreadPool::ThreadPool(uint16_t const thread_count) {

    _workers.resize(thread_count);
    for(auto& worker : _workers) {
        worker.thread = std::thread([&]() {
            while(_exec) {
                if(worker.active_jobs.empty()) {
                    std::unique_lock lock(_mutex);
                    _cond_jobs.wait(lock, [&]() -> bool { return !_exec || !worker.active_jobs.empty(); });
                } else {
                    Job job{};
                    if(worker.active_jobs.try_pop(job)) {
                        job.func();
                        job.finished = 0x1;
                        worker.finished_jobs[job.id] = job;
                        --_active_job_count;
                    }
                }
            }
        });
    }
}

bool ThreadPool::is_finished(JobHandle const& handle) {
    
    auto it = _workers.begin();
    std::advance(it, (handle._id >> 16) & 0xffff);
    return it->finished_jobs[handle._id & 0xffff].finished == 0x1;
}

void ThreadPool::wait(JobHandle const& handle) {

    if(!_exec) {
        throw Exception(u8"Unable to wait for the thread pool task");
    }

    auto it = _workers.begin();
    std::advance(it, (handle._id >> 16) & 0xffff);

    while(it->finished_jobs[handle._id & 0xffff].finished == 0x0) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }
}

void ThreadPool::wait_all() {

    if(!_exec) {
        throw Exception(u8"Unable to wait for the thread pool tasks");
    }

    while(_active_job_count > 0) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }
}

void ThreadPool::join() {

    _exec = false;
    _cond_jobs.notify_all();
    
    for(auto& worker : _workers) {
        worker.thread.join();
    }
}
