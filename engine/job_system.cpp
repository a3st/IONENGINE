// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "job_system.hpp"
#include "precompiled.h"

namespace ionengine
{
    JobWorker::JobWorker(std::atomic<uint64_t>& fence_value)
        : thread(&JobWorker::loop, this), fence_value(&fence_value), running(true)
    {
    }

    JobWorker::~JobWorker()
    {
        running = false;
        thread.join();
    }

    auto JobWorker::loop() -> void
    {
        while (running)
        {
            for (auto& jobs : queues)
            {
                job_func_t func;
                {
                    std::lock_guard lock(mutex);
                    if (!jobs.empty())
                    {
                        func = jobs.front();
                        jobs.pop();
                    }
                    else
                    {
                        std::this_thread::sleep_for(std::chrono::microseconds(1));
                        continue;
                    }
                }
                func();
                ++(*fence_value);
            }
        }
    }

    auto JobWorker::insert(job_func_t&& func, JobQueuePriority const priority) -> void
    {
        std::lock_guard lock(mutex);
        queues[static_cast<uint32_t>(priority)].push(std::move(func));
    }

    JobSystem::JobSystem() : last_index(0)
    {
        uint32_t const thread_count = std::thread::hardware_concurrency() - 1;
        for (uint32_t const i : std::views::iota(0u, thread_count))
        {
            workers.emplace_back(std::make_unique<JobWorker>(worker_fence_value));
        }
    }

    auto JobSystem::submit(job_func_t func, JobQueuePriority const priority) -> JobFuture
    {
        workers[last_index]->insert(std::move(func), priority);
        last_index = (last_index + 1) % static_cast<uint32_t>(workers.size());
        return JobFuture(++sys_fence_value, &sys_fence_value, &worker_fence_value);
    }
} // namespace ionengine