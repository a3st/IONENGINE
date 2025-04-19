// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "upload_manager.hpp"
#include "precompiled.h"

namespace ionengine
{
    UploadManager::UploadManager(core::ref_ptr<rhi::RHI> RHI, uint32_t const numBuffering) : RHI(RHI), bufferIndex(0)
    {
        copyResults.resize(numBuffering);
    }

    auto UploadManager::uploadBuffer(UploadBufferInfo const& uploadBufferInfo) -> void
    {
        UploadElementData uploadElementData{
            .uploadType = UploadType::Buffer,
            .bufferData = {.buffer = uploadBufferInfo.buffer, .offset = uploadBufferInfo.offset},
            .dataBuffer = uploadBufferInfo.dataBytes | std::ranges::to<std::vector<uint8_t>>()};
        uploadElements.emplace_back(std::move(uploadElementData));
    }

    auto UploadManager::uploadTexture(UploadTextureInfo const& uploadTextureInfo) -> void
    {
        UploadElementData uploadElementData{
            .uploadType = UploadType::Texture,
            .textureData = {.texture = uploadTextureInfo.texture, .mipLevel = uploadTextureInfo.mipLevel},
            .dataBuffer = uploadTextureInfo.dataBytes | std::ranges::to<std::vector<uint8_t>>()};
        uploadElements.emplace_back(std::move(uploadElementData));
    }

    auto UploadManager::onExecute() -> void
    {
        copyResults[bufferIndex].wait();

        for (auto const& uploadElement : uploadElements)
        {
            if (uploadElement.uploadType == UploadType::Buffer)
            {
                RHI->getCopyContext()->updateBuffer(uploadElement.bufferData.buffer, uploadElement.bufferData.offset,
                                                    uploadElement.dataBuffer);
            }
            else
            {
                RHI->getCopyContext()->updateTexture(uploadElement.textureData.texture,
                                                     uploadElement.textureData.mipLevel,
                                                     uploadElement.dataBuffer.data());
            }
        }

        if (!uploadElements.empty())
        {
            copyResults[bufferIndex] = RHI->getCopyContext()->execute();
            copyResults[bufferIndex].waitOnContext(RHI->getGraphicsContext());
            bufferIndex = (bufferIndex + 1) % static_cast<uint32_t>(copyResults.size());
        }

        uploadElements.clear();
    }
} // namespace ionengine