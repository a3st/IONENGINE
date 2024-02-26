#pragma once

#include "platform/window.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class Model;
    class Texture;

    template <typename Type>
    struct StreamingData
    {
    };

    struct StreamingData<Model>
    {
        core::ref_ptr<rhi::Buffer> buffer;
        std::vector<uint8_t> data;
    };

    struct StreamingData<Texture>
    {
    };

    using StreamingAsset = std::variant<StreamingData<Model>, StreamingData<Texture>>;

    struct RenderContext
    {
        core::ref_ptr<rhi::Device> device;
        core::ref_ptr<rhi::GraphicsContext> graphics_context;
        core::ref_ptr<rhi::CopyContext> copy_context;
        std::queue<StreamingAsset> streaming;

        rhi::Future<rhi::Query> copy_task;
    };
} // namespace ionengine