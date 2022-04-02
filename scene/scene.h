// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/node_tree.h>
#include <scene/transform_node.h>
#include <lib/thread_pool.h>

namespace ionengine::scene {

class Scene {
public:

    Scene() = default;

    NodeTree& tree() { return _tree; }

    void update_transform(lib::ThreadPool& thread_pool);

private:

    NodeTree _tree;

    struct {
        std::vector<scene::TransformNode*> data;
        std::vector<uint32_t> groups;
    } _transform_cache;

};

}