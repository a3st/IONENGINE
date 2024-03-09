// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "asset_loader.hpp"
#include "linked_device.hpp"
#include "renderer.hpp"
#include "shader_manager.hpp"

namespace ionengine
{
    namespace platform
    {
        class Window;
    }

    class Engine : public core::ref_counted_object
    {
      public:
        Engine(core::ref_ptr<platform::Window> window);

        auto tick() -> void;

        auto run() -> int32_t;

      private:
        core::ref_ptr<platform::Window> window;
        LinkedDevice device;

      protected:
        virtual auto init() -> void = 0;

        virtual auto update(float const dt) -> void = 0;

        virtual auto render() -> void = 0;

        auto create_texture(uint32_t const width, uint32_t const height, TextureFlags const flags)
            -> core::ref_ptr<Texture>;

        auto create_material(std::string_view const shader_name) -> core::ref_ptr<Material>;

        ShaderManager shader_manager;
        AssetLoader asset_loader;
        Renderer renderer;
    };
} // namespace ionengine