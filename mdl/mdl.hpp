// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serialize.hpp"

namespace ionengine
{
    namespace mdl
    {
        std::array<uint8_t, 4> constexpr Magic{'M', 'D', '1', '0'};

        struct BufferData
        {
            uint64_t offset;
            size_t size;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(offset, "offset");
                archive.property(size, "sizeInBytes");
            }
        };

        struct SurfaceData
        {
            uint32_t buffer;
            uint32_t material;
            uint32_t indexCount;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(buffer, "buffer");
                archive.property(material, "material");
                archive.property(indexCount, "indexCount");
            }
        };

        struct ObjectData
        {
            std::string name;
            std::vector<uint32_t> surfaces;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(name, "name");
                archive.property(surfaces, "surfaces");
            }
        };

        

        struct ModelData
        {
            uint32_t materialCount;
            std::vector<ObjectData> objects;
            std::vector<SurfaceData> surfaces;
            std::vector<BufferData> buffers;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(materialCount, "materialCount");
                archive.property(objects, "objects");
                archive.property(surfaces, "surfaces");
                archive.property(buffers, "buffers");
            }
        };
    } // namespace mdl

    struct ModelFile
    {
        std::array<uint8_t, mdl::Magic.size()> magic;
        mdl::ModelData model;
        std::vector<uint8_t> blob;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(magic);
            archive.template with<core::serialize_ojson, core::serialize_ijson>(model);
            archive.property(blob);
        }
    };
}