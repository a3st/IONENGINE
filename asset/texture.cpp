// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/texture.h>
#include <lib/algorithm.h>
#include <dds/DDS.h>

using namespace ionengine;
using namespace ionengine::asset;

lib::Expected<Texture, lib::Result<TextureError>> Texture::create(uint32_t const width, uint32_t const height, bool const is_render_target, lib::math::Color const& initial_color) {

    auto texture = Texture {};
    texture.width = width;
    texture.height = height;
    texture.depth = 1;
    texture.mip_count = 1;
    texture.format = TextureFormat::RGBA8_UNORM;
    texture.filter = TextureFilter::MinMagMipLinear;
    texture.s_address_mode = TextureAddress::Clamp;
    texture.t_address_mode = TextureAddress::Clamp;
    texture.is_render_target = is_render_target;

    if(is_render_target) {
        std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string const render_target_hash = std::format("RT{{{}}}", time);
        texture.hash = XXHash64::hash(render_target_hash.data(), render_target_hash.size(), 0);
    } else {

        std::vector<uint8_t> pixel_data(width * height * 4);
        for(size_t i = 0; i < width; ++i) {
            for(size_t j = 0; j < height; ++j) {
                pixel_data.at((j * width + i) * 4 + 0) = static_cast<uint8_t>(initial_color.r * 255.0f);
                pixel_data.at((j * width + i) * 4 + 1) = static_cast<uint8_t>(initial_color.g * 255.0f);
                pixel_data.at((j * width + i) * 4 + 2) = static_cast<uint8_t>(initial_color.b * 255.0f);
                pixel_data.at((j * width + i) * 4 + 3) = static_cast<uint8_t>(initial_color.a * 255.0f);
            }
        }

        texture.data = std::span<uint8_t const>(pixel_data.data(), pixel_data.size());

        texture.hash = texture.data.hash();
    }

    return lib::make_expected<Texture, lib::Result<TextureError>>(std::move(texture));
}

lib::Expected<Texture, lib::Result<TextureError>> Texture::load_from_file(std::filesystem::path const& file_path) {

    std::filesystem::path const extension = file_path.extension();

    if(extension == ".dds") {

        auto result = lib::load_file(file_path);

        if(result.is_error()) {
            return lib::make_expected<Texture, lib::Result<TextureError>>(
                lib::Result<TextureError> { 
                    .errc = TextureError::IO,
                    .message = "IO error"
                }
            );
        }

        auto file_data = std::move(result.as_ok());
        
        uint64_t offset = 0;

        auto dds_magic_bytes = lib::read_bytes(file_data, offset, sizeof(DirectX::DDS_MAGIC));

        uint32_t dds_magic = *reinterpret_cast<uint32_t const*>(dds_magic_bytes.data());

        if(dds_magic != DirectX::DDS_MAGIC) {
            return lib::make_expected<Texture, lib::Result<TextureError>>(
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

            if(dds_header.caps2 & DDS_CUBEMAP) {
                texture.depth = 6;
                texture.is_cube_map = true;
            } else {
                texture.depth = 1;
            }

            texture.filter = TextureFilter::MinMagMipLinear;
            texture.data = lib::hash::Buffer<uint8_t>(mip_bytes);

            texture.hash = texture.data.hash();
        }
        return lib::make_expected<Texture, lib::Result<TextureError>>(std::move(texture));

    } else {

        return lib::make_expected<Texture, lib::Result<TextureError>>(
            lib::Result<TextureError> { 
                .errc = TextureError::ParseError,
                .message = "Invalid format"
            }
        );
    }
}
