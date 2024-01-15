// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "mesh.hpp"
#include "upload_manager.hpp"

namespace ionengine {

namespace renderer {

class MeshCache {
public:

    MeshCache(Backend& backend, UploadManager& upload_manager);

    auto get(MeshData const& data) -> core::ref_ptr<Texture>;

    auto update(float const dt) -> void;

private:

    Backend* backend;
    UploadManager* upload_manager;
};

}

}