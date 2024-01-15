// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <webgpu/webgpu.hpp>
#include "core/ref_ptr.hpp"
#include "buffer.hpp"
#include "texture.hpp"

namespace ionengine {

namespace renderer {

class Backend;

struct UploadTextureData {
    core::ref_ptr<Texture> texture;
};

using upload_texture_func_t = std::function<void(core::ref_ptr<Texture>)>;

class UploadManager {
public:

    UploadManager(Backend& backend);

    auto upload_texture(UploadTextureData const& data, upload_texture_func_t const& callback) -> void;

private:

    Backend* backend;
    core::ref_ptr<Buffer> upload_buffer{nullptr};
};

};

}