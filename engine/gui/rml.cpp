// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "rml.hpp"
#include "graphics/graphics.hpp"
#include "precompiled.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

        uiColShader = Graphics::createShader("../../assets/shaders/ui_color" + shaderExt + ".bin");
        uiTexShader = Graphics::createShader("../../assets/shaders/ui_texture" + shaderExt + ".bin");
    }

    auto RmlRender::RenderGeometry(Rml::Vertex* vertices, int numVertices, int* indices, int numIndices,
                                   Rml::TextureHandle texture, const Rml::Vector2f& translation) -> void
    {
        core::ref_ptr<Material> material;
        if (texture)
        {
            material = Graphics::createMaterial(uiTexShader);
            material->setValue("inputTexture", core::ref_ptr<Image>(reinterpret_cast<Image*>(texture)));
        }
        else
        {
            material = Graphics::createMaterial(uiColShader);
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
                              .color =
                                  core::Vec4f(vertices[i].colour.red / 255.0f, vertices[i].colour.green / 255.0f,
                                              vertices[i].colour.blue / 255.0f, vertices[i].colour.alpha / 255.0f)};
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

        Rml::Vector2i const contextDimensions = this->GetContext()->GetDimensions();

        DrawParameters drawParams{
            .drawType = DrawType::Triangles,
            .surface = Graphics::createSurface(vertexData, indexData),
            .material = material,
            .renderGroup = RenderGroup::UI,
            .viewMatrix = core::Mat4f::identity(),
            .projMatrix = core::Mat4f::orthographicRH(0, contextDimensions.x, contextDimensions.y, 0, 0.0f, 1.0f),
            .scissorRect = scissorRect};

        auto guiWidget = guiContexts[this->GetContext()];

        Graphics::drawProcedural(drawParams, core::Mat4f::translate(core::Vec3f(translation.x, translation.y, 0.0f)),
                                 guiWidget.get()->getTargetCamera());
    }

    auto RmlRender::EnableScissorRegion(bool isEnable) -> void
    {
        if (!isEnable)
        {
            scissorRect = std::nullopt;
        }
    }

    auto RmlRender::SetScissorRegion(int x, int y, int width, int height) -> void
    {
        scissorRect.emplace(x, y, width, height);
    }

    auto RmlRender::LoadTexture(Rml::TextureHandle& texture, Rml::Vector2i& textureDimensions,
                                const Rml::String& source) -> bool
    {
        int32_t width = 0;
        int32_t height = 0;
        int32_t channels = 0;

        uint8_t* imageDataBytes = ::stbi_load(source.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!imageDataBytes)
        {
            return false;
        }

        auto image = Graphics::createImage(width, height, rhi::TextureFormat::RGBA8_UNORM,
                                           std::span<uint8_t const>(imageDataBytes, width * height * channels));

        ::stbi_image_free(imageDataBytes);

        texture = reinterpret_cast<Rml::TextureHandle>(image.release());
        return true;
    }

    auto RmlRender::ReleaseTexture(Rml::TextureHandle texture) -> void
    {
        delete reinterpret_cast<Image*>(texture);
    }

    auto RmlRender::GenerateTexture(Rml::TextureHandle& texture, const Rml::byte* source,
                                    const Rml::Vector2i& sourceDimensions) -> bool
    {
        auto image =
            Graphics::createImage(sourceDimensions.x, sourceDimensions.y, rhi::TextureFormat::RGBA8_UNORM,
                                  std::span<uint8_t const>(source, sourceDimensions.x * sourceDimensions.y * 4));
        texture = reinterpret_cast<Rml::TextureHandle>(image.release());
        return true;
    }

    RmlSystem::RmlSystem() : beginTime(std::chrono::high_resolution_clock::now())
    {
    }

    auto RmlSystem::GetElapsedTime() -> double
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(endTime - beginTime).count();
    }

    auto RmlSystem::LogMessage(Rml::Log::Type type, const Rml::String& message) -> bool
    {
        std::cout << message.c_str() << std::endl;
        return true;
    }
} // namespace ionengine::internal