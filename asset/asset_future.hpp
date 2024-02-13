// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/job_system.hpp"
#include "core/ref_ptr.hpp"

namespace ionengine
{
    template <typename Type>
    class AssetFuture
    {
      public:
        AssetFuture() : ptr(nullptr)
        {
        }

        AssetFuture(core::ref_ptr<Type> ptr, core::ref_ptr<core::JobSystem> job_system, uint64_t const fence_value)
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
        core::ref_ptr<core::JobSystem> job_system;
        uint64_t fence_value;
    };

} // namespace ionengine