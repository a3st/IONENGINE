// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_tree.h>
#include <scene/transform_node.h>
#include <scene/mesh_node.h>
#include <lib/thread_pool.h>

namespace ionengine::scene {

class Scene {
public:

    Scene() = default;

    SceneTree& tree() { return _tree; }

    void update_transform(lib::ThreadPool& thread_pool);

private:

    SceneTree _tree;

    struct {
        std::vector<Handle<SceneNode>> nodes;
        std::vector<uint32_t> groups;
    } _transform_cache;

};

}
