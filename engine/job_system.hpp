// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine
{
    enum class JobQueuePriority
    {
        Realtime,
        High,
        Normal,
        Low
    };

    class JobFuture
    {
      public:
        JobFuture() : worker_fence_value(nullptr), fence_value(0)
        {
        }

        JobFuture(uint64_t const fence_value, std::atomic<uint64_t>& worker_fence_value)
            : fence_value(fence_value), worker_fence_value(&worker_fence_value)
        {
        }

        JobFuture(JobFuture&& other) : worker_fence_value(other.worker_fence_value), fence_value(other.fence_value)
        {
        }

        JobFuture(JobFuture const&) = delete;

        auto operator=(JobFuture&& other) -> JobFuture&
        {
            worker_fence_value = other.worker_fence_value;
            fence_value = other.fence_value;
            return *this;
        }

        auto operator=(JobFuture const&) -> JobFuture& = delete;

        auto get_result() const -> bool
        {
            return *worker_fence_value >= fence_value;
        }

        auto wait() -> void
        {
            while (*worker_fence_value < fence_value)
            {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        }

      private:
        std::atomic<uint64_t>* worker_fence_value;
        uint64_t fence_value;
    };

    using job_func_t = std::function<void()>;

    class JobWorker
    {
      public:
        JobWorker(std::atomic<uint64_t>& fence_value);

        ~JobWorker();

        auto loop() -> void;

        auto insert(job_func_t&& func, JobQueuePriority const priority) -> void;

      private:
        std::mutex mutex;
        std::atomic<bool> running;
        std::thread thread;
        std::array<std::queue<job_func_t>, 4> queues;
        std::atomic<uint64_t>* fence_value;
    };

    class JobSystem
    {
      public:
        JobSystem();

        auto submit(job_func_t func, JobQueuePriority const priority) -> JobFuture;

      private:
        std::atomic<uint64_t> worker_fence_value;
        std::atomic<uint64_t> sys_fence_value;
        std::vector<std::unique_ptr<JobWorker>> workers;
        uint32_t last_index;
    };
} // namespace ionengine