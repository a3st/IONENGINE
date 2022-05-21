// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/texture.h>
#include <lib/algorithm.h>

using namespace ionengine;
using namespace ionengine::asset;

Texture::Texture(DirectX::DDS_HEADER const& header, std::span<uint8_t const> const mip_data) {

    _width = header.width;
    _height = header.height;
    _mip_count = header.mipMapCount;

    if(header.ddspf.flags & DDS_FOURCC) {
        if(header.ddspf.fourCC == MAKEFOURCC('D', 'X', 'T', '1')) {
            _format = TextureFormat::BC1;
        } else if(header.ddspf.fourCC == MAKEFOURCC('D', 'X', 'T', '3')) {
            _format = TextureFormat::BC2;
        } else if(header.ddspf.fourCC == MAKEFOURCC('D', 'X', 'T', '5')) {
            _format = TextureFormat::BC3;
        } else if(header.ddspf.fourCC == MAKEFOURCC('D', 'X', 'T', '2')) {
            _format = TextureFormat::BC2;
        } else if(header.ddspf.fourCC == MAKEFOURCC('D', 'X', 'T', '4')) {
            _format = TextureFormat::BC3;
        } else if(header.ddspf.fourCC == MAKEFOURCC('B', 'C', '4', 'U')) {
            _format = TextureFormat::BC4;
        } else if(header.ddspf.fourCC == MAKEFOURCC('B', 'C', '4', 'S')) {
            _format = TextureFormat::BC4;
        } else if(header.ddspf.fourCC == MAKEFOURCC('B', 'C', '5', 'U')) {
            _format = TextureFormat::BC5;
        } else if(header.ddspf.fourCC == MAKEFOURCC('B', 'C', '5', 'S')) {
            _format = TextureFormat::BC5;
        }
    } else if(header.ddspf.flags & DDS_RGB) {

    } else if (header.ddspf.flags & DDS_RGB) {
        switch(header.ddspf.RGBBitCount) {
            case 32: {
                if(ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000)) {
                    _format = TextureFormat::RGBA8_UNORM;
                }
            } break;
        }
    }

    _filter = TextureFilter::MinMagMipLinear;
    _data = lib::hash::Buffer<uint8_t>(mip_data);
}

lib::Expected<Texture, lib::Result<TextureError>> Texture::load_from_file(std::filesystem::path const& file_path) {

    std::filesystem::path extension = file_path.extension();

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

        return lib::Expected<Texture, lib::Result<TextureError>>::ok(Texture(dds_header, mip_bytes));

    } else {

        return lib::Expected<Texture, lib::Result<TextureError>>::error(
            lib::Result<TextureError> { 
                .errc = TextureError::ParseError,
                .message = "Invalid format"
            }
        );
    }
    
}

uint64_t Texture::hash() const {
    return _data.hash();
}

void Texture::cache_entry(size_t const value) {
    _cache_entry = value;
}

size_t Texture::cache_entry() const {
    return _cache_entry;
}

TextureFormat Texture::format() const {
    return _format;
}

uint32_t Texture::width() const {
    return _width;
}

uint32_t Texture::height() const {
    return _height;
}

uint32_t Texture::mip_count() const {
    return _mip_count;
}

std::span<uint8_t const> Texture::data() const {
    return std::span<uint8_t const>(_data.data(), _data.size());
}
