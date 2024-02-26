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

    class JobSystem : public core::ref_counted_object
    {
      public:
        JobSystem();

        auto submit(job_func_t func, JobQueuePriority const priority) -> uint64_t;

        auto wait(uint64_t const fence_value) -> void;

        auto get_result(uint64_t const fence_value) const -> bool
        {
            return worker_fence_value >= fence_value;
        }

      private:
        std::atomic<uint64_t> worker_fence_value;
        std::atomic<uint64_t> sys_fence_value;
        std::vector<std::unique_ptr<JobWorker>> workers;
        uint32_t last_index;
    };
} // namespace ionengine