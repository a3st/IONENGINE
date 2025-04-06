// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    struct UploadBufferInfo
    {
        core::ref_ptr<rhi::Buffer> buffer;
        uint64_t offset;
        std::span<uint8_t const> dataBytes;
    };

    struct UploadTextureInfo
    {
        core::ref_ptr<rhi::Texture> texture;
        uint32_t mipLevel;
        std::span<uint8_t const> dataBytes;
    };

    using UploadCompletedCallback = std::function<void()>;

    class UploadManager
    {
      public:
        UploadManager(core::ref_ptr<rhi::RHI> RHI);

        auto uploadBuffer(UploadBufferInfo const& uploadBufferInfo, UploadCompletedCallback&& completedCallback)
            -> void;

        auto uploadTexture(UploadTextureInfo const& uploadTextureInfo, UploadCompletedCallback&& completedCallback)
            -> void;

        auto onExecuteBuffers(bool const waitAfterExecute) -> void;

        auto onExecuteTextures() -> void;

      private:
        core::ref_ptr<rhi::RHI> RHI;

        struct UploadBufferData
        {
            core::ref_ptr<rhi::Buffer> buffer;
            uint64_t offset;
            std::vector<uint8_t> dataBuffer;
            UploadCompletedCallback callback;
        };

        struct UploadTextureData
        {
            core::ref_ptr<rhi::Texture> texture;
            uint32_t mipLevel;
            std::vector<uint8_t> dataBuffer;
            UploadCompletedCallback callback;
        };

        struct TrackingBufferData
        {
            rhi::Future<rhi::Buffer> future;
            UploadCompletedCallback callback;
        };

        struct TrackingTextureData
        {
            rhi::Future<rhi::Texture> future;
            UploadCompletedCallback callback;
        };

        std::queue<UploadBufferData> bufferUploads;
        std::vector<TrackingBufferData> trackingBufferUploads;
        std::queue<UploadTextureData> textureUploads;
        std::vector<TrackingTextureData> trackingTextureUploads;

        rhi::Future<void> executeResult;

        inline static size_t constexpr kTextureDispatchMaxSize = 4 * 1024 * 1024;
    };
} // namespace ionengine