// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/texture.h>
#include <lib/algorithm.h>
#include <dds/DDS.h>

using namespace ionengine;
using namespace ionengine::asset;

lib::Expected<Texture, lib::Result<TextureError>> Texture::load_from_file(std::filesystem::path const& file_path) {

    std::filesystem::path const extension = file_path.extension();

    if(extension == ".dds") {

        auto result = lib::load_file(file_path);

        if(result.is_error()) {
            return lib::Expected<Texture, lib::Result<TextureError>>::error(
                lib::Result<TextureError> { 
                    .errc = TextureError::IO,
                    .message = "IO error"
                }
            );
        }

        auto file_data = std::move(result.value());
        
        uint64_t offset = 0;

        auto dds_magic_bytes = lib::read_bytes(file_data, offset, sizeof(DirectX::DDS_MAGIC));

        uint32_t dds_magic = *reinterpret_cast<uint32_t const*>(dds_magic_bytes.data());

        if(dds_magic != DirectX::DDS_MAGIC) {
            return lib::Expected<Texture, lib::Result<TextureError>>::error(
                lib::Result<TextureError> { 
                    .errc = TextureError::ParseError,
                    .message = "Invalid format"
                }
            );
        }

        auto dds_header_bytes = lib::read_bytes(file_data, offset, sizeof(DirectX::DDS_HEADER));

        auto dds_header = DirectX::DDS_HEADER {};
        std::memcpy(&dds_header, dds_header_bytes.data(), dds_header_bytes.size_bytes());

        auto mip_bytes = lib::read_bytes(file_data, offset, file_data.size() - offset);

        auto texture = Texture {};
        {
            texture.width = dds_header.width;
            texture.height = dds_header.height;
            texture.mip_count = dds_header.mipMapCount;

            if(dds_header.ddspf.flags & DDS_FOURCC) {
                if(dds_header.ddspf.fourCC == MAKEFOURCC('D', 'X', 'T', '1')) {
                    texture.format = TextureFormat::BC1;
                } else if(dds_header.ddspf.fourCC == MAKEFOURCC('D', 'X', 'T', '3')) {
                    texture.format = TextureFormat::BC2;
                } else if(dds_header.ddspf.fourCC == MAKEFOURCC('D', 'X', 'T', '5')) {
                    texture.format = TextureFormat::BC3;
                } else if(dds_header.ddspf.fourCC == MAKEFOURCC('D', 'X', 'T', '2')) {
                    texture.format = TextureFormat::BC2;
                } else if(dds_header.ddspf.fourCC == MAKEFOURCC('D', 'X', 'T', '4')) {
                    texture.format = TextureFormat::BC3;
                } else if(dds_header.ddspf.fourCC == MAKEFOURCC('B', 'C', '4', 'U')) {
                    texture.format = TextureFormat::BC4;
                } else if(dds_header.ddspf.fourCC == MAKEFOURCC('B', 'C', '4', 'S')) {
                    texture.format = TextureFormat::BC4;
                } else if(dds_header.ddspf.fourCC == MAKEFOURCC('B', 'C', '5', 'U')) {
                    texture.format = TextureFormat::BC5;
                } else if(dds_header.ddspf.fourCC == MAKEFOURCC('B', 'C', '5', 'S')) {
                    texture.format = TextureFormat::BC5;
                }
            } else if(dds_header.ddspf.flags & DDS_RGB) {

            } else if (dds_header.ddspf.flags & DDS_RGB) {
                switch(dds_header.ddspf.RGBBitCount) {
                    case 32: {
                        if(ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000)) {
                            texture.format = TextureFormat::RGBA8_UNORM;
                        }
                    } break;
                }
            }

            texture.filter = TextureFilter::MinMagMipLinear;
            texture.data = lib::hash::Buffer<uint8_t>(mip_bytes);

            texture.hash = texture.data.hash();
        }
        return lib::Expected<Texture, lib::Result<TextureError>>::ok(std::move(texture));

    } else {

        return lib::Expected<Texture, lib::Result<TextureError>>::error(
            lib::Result<TextureError> { 
                .errc = TextureError::ParseError,
                .message = "Invalid format"
            }
        );
    }
}
