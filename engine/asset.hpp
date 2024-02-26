// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "job_system.hpp"

namespace ionengine
{
    template <typename Type>
    class AssetFuture
    {
      public:
        AssetFuture() : ptr(nullptr)
        {
        }

        AssetFuture(core::ref_ptr<Type> ptr, core::ref_ptr<JobSystem> job_system, uint64_t const fence_value)
            : ptr(ptr), job_system(job_system), fence_value(fence_value)
        {
        }

        AssetFuture(AssetFuture&& other)
            : ptr(std::move(other.ptr)), job_system(std::move(other.job_system)), fence_value(fence_value)
        {
        }

        AssetFuture(AssetFuture const&) = delete;

        auto operator=(AssetFuture&& other) -> AssetFuture&
        {
            ptr = std::move(other.ptr);
            job_system = std::move(other.job_system);
            fence_value = fence_value;
            return *this;
        }

        auto get_result() const -> bool
        {
            return job_system->get_result(fence_value);
        }

        auto wait() -> void
        {
            job_system->wait(fence_value);
        }

        auto get() -> core::ref_ptr<Type>
        {
            if (!get_result())
            {
                wait();
            }
            return ptr;
        }

      private:
        core::ref_ptr<Type> ptr;
        core::ref_ptr<JobSystem> job_system;
        uint64_t fence_value;
    };

    class Asset : public core::ref_counted_object
    {
    };
} // namespace ionengine