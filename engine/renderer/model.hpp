// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serialize.hpp"
#include "rhi/rhi.hpp"

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
            uint32_t ibuffer;
            uint32_t material;
            uint32_t vertexCount;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(ibuffer, "ibuffer");
                archive.property(material, "material");
                archive.property(vertexCount, "vertexCount");
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

        struct DescriptionData
        {
            uint32_t materialCount;
            std::vector<ObjectData> objects;
            std::vector<SurfaceData> surfaces;
            std::vector<BufferData> ibuffers;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(materialCount, "materialCount");
                archive.property(objects, "objects");
                archive.property(surfaces, "surfaces");
                archive.property(ibuffers, "ibuffers");
            }
        };
    } // namespace mdl

    struct ModelFile
    {
        std::array<uint8_t, mdl::Magic.size()> magic;
        mdl::DescriptionData description;
        std::vector<uint8_t> vblob;
        std::vector<uint8_t> iblob;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(magic);
            archive.template with<core::serialize_ojson, core::serialize_ijson>(description);
            archive.property(vblob);
            archive.property(iblob);
        }
    };

    class Surface : public core::ref_counted_object
    {
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        core::ref_ptr<rhi::Buffer> indexBuffer;
        uint32_t vertexCount;
    };

    class Model : public core::ref_counted_object
    {
      public:
        Model(rhi::Device& device);
    };
} // namespace ionengine