// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/linked_device.hpp"

namespace ionengine
{
    namespace platform
    {
        class Window;
    }

    class ShaderAsset;

    class Engine : public core::ref_counted_object
    {
      public:
        Engine(platform::Window* window);

        auto tick() -> void;

        auto loop() -> void;

        auto run() -> void;

      private:
        platform::Window* window;
        LinkedDevice device;

      protected:
        virtual auto init() -> void = 0;

        virtual auto update(float const dt) -> void = 0;

        virtual auto render() -> void = 0;

        auto createShaderAsset() -> core::ref_ptr<ShaderAsset>;

        /*auto create_texture(uint32_t const width, uint32_t const height, TextureFlags const flags)
            -> core::ref_ptr<Texture>;

        auto create_material(std::string_view const shader_name) -> core::ref_ptr<Material>;*/

        //ShaderManager shader_manager;
        //AssetLoader asset_loader;
        //Renderer renderer;
    };
} // namespace ionengine