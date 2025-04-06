// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "cmp.hpp"
#include "precompiled.h"
#include <compressonator.h>

namespace ionengine::asset
{
    CMPImporter::CMPImporter(bool const generateMipMaps) : generateMipMaps(generateMipMaps)
    {
        ::CMP_InitFramework();
    }

    auto CMPImporter::loadFromFile(std::filesystem::path const& filePath) -> std::expected<TextureFile, core::error>
    {
        CMP_MipSet srcMipSet{};
        CMP_ERROR error = ::CMP_LoadTexture(filePath.string().c_str(), &srcMipSet);
        if (error != CMP_OK)
        {
            return std::unexpected(core::error(
                core::error_code::eof, "the input file is in a different format, is corrupted, or was not found"));
        }

        txe::TextureData textureData{.format = txe::TextureFormat::RGBA8_UNORM,
                                     .dimension = txe::TextureDimension::_2D};
        std::basic_stringstream<uint8_t> textureBlob;

        if (srcMipSet.m_nMipLevels <= 1 && generateMipMaps)
        {
            int32_t const minMipSize =
                ::CMP_CalcMinMipSize(srcMipSet.m_nHeight, srcMipSet.m_nWidth, srcMipSet.m_nMaxMipLevels);
            ::CMP_GenerateMIPLevels(&srcMipSet, minMipSize);
        }

        textureData.mipLevelCount = std::max(1, srcMipSet.m_nMaxMipLevels - 1);

        for (uint32_t const i : std::views::iota(0u, textureData.mipLevelCount))
        {
            CMP_MipLevel* mipLevel;
            ::CMP_GetMipLevel(&mipLevel, &srcMipSet, i, 0);

            if (i == 0)
            {
                textureData.width = mipLevel->m_nWidth;
                textureData.height = mipLevel->m_nHeight;
            }

            uint64_t const offset = textureBlob.tellp();
            textureBlob.write(mipLevel->m_pbData, mipLevel->m_dwLinearSize);

            txe::BufferData bufferData{.offset = offset, .size = mipLevel->m_dwLinearSize};
            textureData.buffers.emplace_back(std::move(bufferData));
        }

        ::CMP_FreeMipSet(&srcMipSet);

        return TextureFile{.magic = txe::Magic,
                           .textureData = std::move(textureData),
                           .blob = {std::istreambuf_iterator<uint8_t>(textureBlob.rdbuf()), {}}};
    }
} // namespace ionengine::asset