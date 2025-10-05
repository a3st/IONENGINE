// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "cmp.hpp"
#include "precompiled.h"
#include <compressonator.h>

namespace ionengine::asset
{
    CMPImporter::CMPImporter(bool const generateMipMaps) : _generateMipMaps(generateMipMaps)
    {
        ::CMP_InitFramework();
    }

    auto CMPImporter::loadFromFile(std::filesystem::path const& filePath) -> std::expected<TextureFile, core::error>
    {
        CMP_MipSet srcMipSet{};
        CMP_ERROR error = ::CMP_LoadTexture(filePath.string().c_str(), &srcMipSet);
        if (error != CMP_OK)
        {
            return std::unexpected(core::error("Failed to open file"));
        }

        std::vector<asset::txe::BufferData> textureBuffers;
        std::basic_stringstream<uint8_t> textureBlob;

        if (srcMipSet.m_nMipLevels <= 1 && _generateMipMaps)
        {
            int32_t const minMipSize =
                ::CMP_CalcMinMipSize(srcMipSet.m_nHeight, srcMipSet.m_nWidth, srcMipSet.m_nMaxMipLevels);
            ::CMP_GenerateMIPLevels(&srcMipSet, minMipSize);
        }

        uint32_t const width = srcMipSet.dwWidth;
        uint32_t const height = srcMipSet.dwHeight;
        uint32_t const mipLevelCount = std::max(1, srcMipSet.m_nMaxMipLevels - 1);

        for (uint32_t const i : std::views::iota(0u, mipLevelCount))
        {
            CMP_MipLevel* mipLevel;
            ::CMP_GetMipLevel(&mipLevel, &srcMipSet, i, 0);

            txe::BufferData bufferData{.offset = static_cast<uint64_t>(textureBlob.tellp()),
                                       .size = mipLevel->m_dwLinearSize};
            textureBuffers.emplace_back(std::move(bufferData));

            textureBlob.write(mipLevel->m_pbData, mipLevel->m_dwLinearSize);
        }

        ::CMP_FreeMipSet(&srcMipSet);

        txe::TextureData textureData{.format = txe::TextureFormat::RGBA8_UNORM,
                                     .dimension = txe::TextureDimension::_2D,
                                     .width = width,
                                     .height = height,
                                     .mipLevelCount = mipLevelCount,
                                     .buffers = std::move(textureBuffers)};
        return TextureFile{.magic = txe::Magic,
                           .textureData = std::move(textureData),
                           .textureBlob = {std::istreambuf_iterator<uint8_t>(textureBlob.rdbuf()), {}}};
    }
} // namespace ionengine::asset