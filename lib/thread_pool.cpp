
#include <precompiled.h>
#include <lib/thread_pool.h>

using namespace ionengine;
using namespace ionengine::lib;

ThreadPool::ThreadPool(uint32_t const thread_count) 
    : _thread_count(thread_count), _workers(std::make_unique<Worker[]>(thread_count)), _last_pending_jobs(std::make_unique<uint32_t[]>(thread_count)), _exec(true) {

    for(uint32_t i = 0; i < thread_count; ++i) {

        _workers[i].thread = std::thread(
            [&](Worker& worker) {

                while(_exec) {
                    if(worker.pending_jobs.empty()) {

                        std::this_thread::sleep_for(std::chrono::nanoseconds(1000));

                    } else {

                        auto job_data = Job {};

                        if(worker.pending_jobs.try_pop(job_data)) {

                            job_data.func();

                            worker.finished_jobs[job_data.id].gen.store(job_data.gen);
                            worker.finished_jobs[job_data.id].is_finished.store(true);
                            
                            _pending_jobs_count.store(_pending_jobs_count.load() - 1);
                        }
                    }
                }
            },
            std::ref(_workers[i])
        );
    }
}

bool ThreadPool::is_finished(Handle<Job> const& job_data) const {

    assert(_exec && "An error occurred while waiting for the completion of task in the thread pool");
    
    uint16_t const hiword = static_cast<uint16_t>(job_data.id >> 16);
    uint16_t const loword = static_cast<uint16_t>(job_data.id);
    uint8_t const thread_index = static_cast<uint8_t>(hiword >> 8);
    uint8_t const generation_index = static_cast<uint8_t>(hiword);

    auto const& job_stream = _workers[static_cast<uint32_t>(thread_index)].finished_jobs[static_cast<uint32_t>(loword)];
    return job_stream.is_finished.load() && job_stream.gen.load() == static_cast<uint32_t>(generation_index);
}

void ThreadPool::wait(Handle<Job> const& job_data) const {

    assert(_exec && "An error occurred while waiting for the completion of task in the thread pool");

    uint16_t const hiword = static_cast<uint16_t>(job_data.id >> 16);
    uint16_t const loword = static_cast<uint16_t>(job_data.id);
    uint8_t const thread_index = static_cast<uint8_t>(hiword >> 8);
    uint8_t const generation_index = static_cast<uint8_t>(hiword);

    auto const& job_stream = _workers[static_cast<uint32_t>(thread_index)].finished_jobs[static_cast<uint32_t>(loword)];

    while(!(job_stream.is_finished.load() && job_stream.gen.load() == static_cast<uint32_t>(generation_index))) {
        
        std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
    }
}

void ThreadPool::wait_all() const {

    assert(_exec && "An error occurred while waiting for the completion of tasks in the thread pool");

    while(_pending_jobs_count.load() > 0) {

        std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
    }
}

void ThreadPool::join() {

    _exec = false;
    
    for(uint32_t i = 0; i < _thread_count; ++i) {
        _workers[i].thread.join();
    }
}
