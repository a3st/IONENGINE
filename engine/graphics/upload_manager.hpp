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

    class UploadManager
    {
      public:
        UploadManager(core::ref_ptr<rhi::RHI> RHI, uint32_t const numBuffering);

        auto uploadBuffer(UploadBufferInfo const& uploadBufferInfo) -> void;

        auto uploadTexture(UploadTextureInfo const& uploadTextureInfo) -> void;

        auto onExecute() -> void;

      private:
        core::ref_ptr<rhi::RHI> RHI;

        enum class UploadType
        {
            Buffer,
            Texture
        };

        struct UploadElementData
        {
            UploadType uploadType;

            struct
            {
                core::ref_ptr<rhi::Buffer> buffer;
                uint64_t offset;
            } bufferData;
            struct
            {
                core::ref_ptr<rhi::Texture> texture;
                uint32_t mipLevel;
            } textureData;

            std::vector<uint8_t> dataBuffer;
        };

        std::vector<UploadElementData> uploadElements;

        std::vector<rhi::Future<void>> copyResults;
        uint32_t bufferIndex;
    };
} // namespace ionengine