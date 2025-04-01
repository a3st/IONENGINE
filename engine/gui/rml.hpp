// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graphics/buffer_allocator.hpp"
#include "graphics/shader.hpp"
#include "rhi/rhi.hpp"
#include <RmlUi/Core.h>

namespace ionengine::internal
{
    class RmlRender : public Rml::RenderInterface
    {
      public:
        RmlRender(core::ref_ptr<rhi::RHI> RHI);

        virtual ~RmlRender() = default;

        auto RenderGeometry(Rml::Vertex* vertices, int numVertices, int* indices, int numIndices,
                            Rml::TextureHandle textureHandle, const Rml::Vector2f& translation) -> void override;

        auto EnableScissorRegion(bool isEnable) -> void override;

        auto SetScissorRegion(int x, int y, int width, int height) -> void override;

        auto LoadTexture(Rml::TextureHandle& textureHandle, Rml::Vector2i& textureDimensions, const Rml::String& source)
            -> bool override;

        auto ReleaseTexture(Rml::TextureHandle textureHandle) -> void override;

      private:
        core::ref_ptr<rhi::RHI> RHI;

        core::ref_ptr<Shader> uiColShader;
        core::ref_ptr<Shader> uiTexShader;

        core::ref_ptr<BufferAllocator> verticesBufferAllocator;
        core::ref_ptr<BufferAllocator> indicesBufferAllocator;
    };
} // namespace ionengine::internal