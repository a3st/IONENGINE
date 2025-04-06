// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "image.hpp"
#include "precompiled.h"
#include "upload_manager.hpp"

namespace ionengine
{
    auto FXTextureFormat_to_RHITextureFormat(asset::txe::TextureFormat const format) -> rhi::TextureFormat
    {
        switch (format)
        {
            case asset::txe::TextureFormat::RGBA8_UNORM:
                return rhi::TextureFormat::RGBA8_UNORM;
            default:
                throw std::invalid_argument("unknown TextureFormat for passed argument");
        }
    }

    Image::Image(rhi::RHI& RHI, UploadManager& uploadManager, asset::TextureFile const& textureFile)
    {
        rhi::TextureCreateInfo const textureCreateInfo{
            .width = textureFile.textureData.width,
            .height = textureFile.textureData.height,
            .depth = 1,
            .mipLevels = textureFile.textureData.mipLevelCount,
            .format = FXTextureFormat_to_RHITextureFormat(textureFile.textureData.format),
            .dimension = rhi::TextureDimension::_2D,
            .flags = (rhi::TextureUsageFlags)(rhi::TextureUsage::ShaderResource | rhi::TextureUsage::CopyDest)};
        texture = RHI.createTexture(textureCreateInfo);

        for (uint32_t const i : std::views::iota(0u, textureFile.textureData.mipLevelCount))
        {
            auto const& bufferData = textureFile.textureData.buffers[i];

            UploadTextureInfo const uploadTextureInfo{
                .texture = texture,
                .mipLevel = i,
                .dataBytes = std::span<uint8_t const>(textureFile.blob.data() + bufferData.offset, bufferData.size)};
            uploadManager.uploadTexture(uploadTextureInfo, [this, &RHI]() -> void {
                RHI.getGraphicsContext()->barrier(texture, rhi::ResourceState::Common, rhi::ResourceState::ShaderRead);
            });
        }
    }

    auto Image::getTexture() const -> core::ref_ptr<rhi::Texture>
    {
        return texture;
    }
} // namespace ionengine