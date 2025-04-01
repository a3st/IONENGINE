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
        // DrawParameters drawParams{.drawType = DrawType::Triangles};

        // Graphics::drawProcedural()
    }
} // namespace ionengine::internal