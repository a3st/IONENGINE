// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "upload_manager.hpp"
#include "precompiled.h"

namespace ionengine
{
    UploadManager::UploadManager(core::ref_ptr<rhi::RHI> RHI, uint32_t const numBuffering) : RHI(RHI), bufferIndex(0)
    {
        executeResults.resize(numBuffering);
    }

    auto UploadManager::uploadBuffer(UploadBufferInfo const& uploadBufferInfo,
                                     UploadCompletedCallback&& completedCallback) -> void
    {
        UploadElementData uploadElementData{
            .uploadType = UploadType::Buffer,
            .bufferData = {.buffer = uploadBufferInfo.buffer, .offset = uploadBufferInfo.offset},
            .dataBuffer = uploadBufferInfo.dataBytes | std::ranges::to<std::vector<uint8_t>>(),
            .callback = std::move(completedCallback)};
        uploadElements.emplace(std::move(uploadElementData));
    }

    auto UploadManager::uploadTexture(UploadTextureInfo const& uploadTextureInfo,
                                      UploadCompletedCallback&& completedCallback) -> void
    {
        UploadElementData uploadElementData{
            .uploadType = UploadType::Texture,
            .textureData = {.texture = uploadTextureInfo.texture, .mipLevel = uploadTextureInfo.mipLevel},
            .dataBuffer = uploadTextureInfo.dataBytes | std::ranges::to<std::vector<uint8_t>>(),
            .callback = std::move(completedCallback)};
        uploadElements.emplace(std::move(uploadElementData));
    }

    auto UploadManager::onExecute() -> void
    {
        while (!uploadElements.empty())
        {
            UploadElementData uploadElementData = std::move(uploadElements.front());
            uploadElements.pop();

            TrackElementData trackElementData{.uploadType = uploadElementData.uploadType,
                                              .callback = std::move(uploadElementData.callback)};

            if (uploadElementData.uploadType == UploadType::Buffer)
            {
                rhi::Future<rhi::Buffer> future = RHI->getCopyContext()->updateBuffer(
                    uploadElementData.bufferData.buffer, uploadElementData.bufferData.offset,
                    uploadElementData.dataBuffer);

                trackElementData.bufferFuture = std::move(future);
            }
            else
            {
                rhi::Future<rhi::Texture> future = RHI->getCopyContext()->updateTexture(
                    uploadElementData.textureData.texture, uploadElementData.textureData.mipLevel,
                    uploadElementData.dataBuffer.data());

                trackElementData.textureFuture = std::move(future);
            }

            trackElements.emplace_back(std::move(trackElementData));
        }

        if (!trackElements.empty())
        {
            auto executeResult = RHI->getCopyContext()->execute();
            executeResult.waitOnContext(RHI->getGraphicsContext());
            executeResults[bufferIndex] = std::move(executeResult);

            bufferIndex = (bufferIndex + 1) % static_cast<uint32_t>(executeResults.size());
        }
    }

    auto UploadManager::onComplete() -> void
    {
        std::erase_if(trackElements, [](auto& x) {
            bool isCompleted = false;

            if (x.uploadType == UploadType::Buffer)
            {
                if (x.bufferFuture.getResult())
                {
                    isCompleted = true;
                }
            }
            else
            {
                if (x.textureFuture.getResult())
                {
                    isCompleted = true;
                }
            }

            if (isCompleted)
            {
                x.callback();
            }

            return isCompleted;
        });
    }
} // namespace ionengine