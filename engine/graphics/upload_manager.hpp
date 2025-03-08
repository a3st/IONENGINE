// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine::internal
{
    struct UploadBufferInfo
    {
        core::ref_ptr<rhi::Buffer> buffer;
        uint64_t offset;
        std::span<uint8_t const> dataBytes;
    };

    using UploadCompletedCallback = std::function<void()>;

    class UploadManager
    {
      public:
        UploadManager(core::ref_ptr<rhi::RHI> RHI) : RHI(RHI)
        {
        }

        auto uploadBufferToDevice(UploadBufferInfo const& uploadBufferInfo, UploadCompletedCallback&& completedCallback)
            -> void;

        auto uploadTextureToDevice() -> void;

        auto onExecuteTask() -> void;

      private:
        core::ref_ptr<rhi::RHI> RHI;

        struct UploadBufferData
        {
            core::ref_ptr<rhi::Buffer> buffer;
            uint64_t offset;
            std::vector<uint8_t> dataBuffer;
            UploadCompletedCallback callback;
        };

        struct TrackingBufferData
        {
            rhi::Future<rhi::Buffer> future;
            UploadCompletedCallback callback;
        };

        std::queue<UploadBufferData> bufferUploads;
        std::vector<TrackingBufferData> trackingBufferUploads;

        size_t const UploadManagerDispatchMaxSize = 4 * 1024 * 1024;
    };
} // namespace ionengine::internal