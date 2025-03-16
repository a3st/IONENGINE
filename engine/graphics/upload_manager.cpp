// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "upload_manager.hpp"
#include "precompiled.h"

namespace ionengine::internal
{
    UploadManager::UploadManager(core::ref_ptr<rhi::RHI> RHI) : RHI(RHI)
    {
    }

    auto UploadManager::uploadBuffer(UploadBufferInfo const& uploadBufferInfo,
                                     UploadCompletedCallback&& completedCallback) -> void
    {
        UploadBufferData uploadBufferData{.buffer = uploadBufferInfo.buffer,
                                          .offset = uploadBufferInfo.offset,
                                          .dataBuffer =
                                              uploadBufferInfo.dataBytes | std::ranges::to<std::vector<uint8_t>>(),
                                          .callback = std::move(completedCallback)};
        bufferUploads.emplace(std::move(uploadBufferData));
    }

    auto UploadManager::onExecuteTask(bool const onlyBuffers, bool const waitAfterExecute) -> void
    {
        bool isUploaded = false;

        for (auto const& trackingBufferUpload : trackingBufferUploads)
        {
            if (!trackingBufferUpload.future.getResult())
            {
                std::cout << "Wait for upload" << std::endl;
                return;
            }

            trackingBufferUpload.callback();
            isUploaded = true;
        }

        if (isUploaded)
        {
            trackingBufferUploads.clear();
        }

        auto copyContext = RHI->getCopyContext();
        size_t dispatchSize = 0;

        while (!bufferUploads.empty())
        {
            UploadBufferData uploadBufferData = std::move(bufferUploads.front());

            bufferUploads.pop();
            dispatchSize += uploadBufferData.dataBuffer.size();

            rhi::Future<rhi::Buffer> future = copyContext->updateBuffer(
                uploadBufferData.buffer, uploadBufferData.offset, uploadBufferData.dataBuffer);

            TrackingBufferData trackingBufferData{.future = std::move(future),
                                                  .callback = std::move(uploadBufferData.callback)};
            trackingBufferUploads.emplace_back(std::move(trackingBufferData));
        }

        if (!trackingBufferUploads.empty())
        {
            rhi::Future<void> executeResult = copyContext->execute();
            if (waitAfterExecute)
            {
                executeResult.wait();
            }
        }
    }
} // namespace ionengine::internal