// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "rml.hpp"
#include "graphics/graphics.hpp"
#include "precompiled.h"

namespace ionengine::internal
{
    RmlRender::RmlRender(core::ref_ptr<rhi::RHI> RHI) : RHI(RHI)
    {
        std::string shaderExt;
        if (RHI->getName().compare("D3D12") == 0)
        {
            shaderExt = "_pc";
        }
        else
        {
            shaderExt = "_vk";
        }

        uiColShader = Graphics::loadShaderFromFile("../../assets/shaders/ui_color" + shaderExt + ".bin");
        uiTexShader = Graphics::loadShaderFromFile("../../assets/shaders/ui_texture" + shaderExt + ".bin");
    }

    auto RmlRender::RenderGeometry(Rml::Vertex* vertices, int numVertices, int* indices, int numIndices,
                                   Rml::TextureHandle textureHandle, const Rml::Vector2f& translation) -> void
    {
        if (textureHandle)
        {
        }

        struct Vertex
        {
            core::Vec2f position;
            core::Vec2f uv;
            core::Vec4f color;
        };

        std::vector<uint8_t> vertexData(sizeof(Vertex) * numVertices);
        {
            std::basic_ospanstream<uint8_t> stream(vertexData);
            for (int32_t const i : std::views::iota(0, numVertices))
            {
                Vertex vertex{.position = core::Vec2f(vertices[i].position.x, vertices[i].position.y),
                              .uv = core::Vec2f(vertices[i].tex_coord.x, vertices[i].tex_coord.y),
                              .color = core::Vec4f(vertices[i].colour.red, vertices[i].colour.green,
                                                   vertices[i].colour.red, vertices[i].colour.alpha)};
                stream.write(reinterpret_cast<uint8_t*>(&vertex), sizeof(Vertex));
            }
        }

        std::vector<uint8_t> indexData(sizeof(uint32_t) * numIndices);
        {
            std::basic_ospanstream<uint8_t> stream(indexData);
            for (int32_t const i : std::views::iota(0, numIndices))
            {
                stream.write(reinterpret_cast<uint8_t*>(&indices[i]), sizeof(uint32_t));
            }
        }

        core::ref_ptr<Surface> surface = Graphics::createSurface(vertexData, indexData);
        core::ref_ptr<Material> material = Graphics::createMaterial(uiColShader);

        DrawParameters drawParams{
            .drawType = DrawType::Triangles, .surface = surface, .material = material, .renderGroup = RenderGroup::UI};

        auto guiContext = guiContexts[this->GetContext()];

        Graphics::drawProcedural(drawParams, core::Mat4f::translate(core::Vec3f(translation.x, translation.y, 0.0f)),
                                 guiContext->getTargetCamera());
    }

    auto RmlRender::EnableScissorRegion(bool isEnable) -> void
    {
    }

    auto RmlRender::SetScissorRegion(int x, int y, int width, int height) -> void
    {
    }

    auto RmlRender::LoadTexture(Rml::TextureHandle& textureHandle, Rml::Vector2i& textureDimensions,
                                const Rml::String& source) -> bool
    {
        return false;
    }

    auto RmlRender::ReleaseTexture(Rml::TextureHandle textureHandle) -> void
    {
    }
} // namespace ionengine::internal