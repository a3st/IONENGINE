// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene.h>

using namespace ionengine::scene;

void Scene::update_transform(lib::ThreadPool& thread_pool) {

    Handle<Node> cur_node = _tree.root();

    while(cur_node != INVALID_HANDLE(Node)) {

        size_t const child_size = _tree.node<Node>(cur_node)._childrens.size();

        for(size_t i = 0; i < child_size; ++i) {

            _transform_cache.data.emplace_back(&_tree.node<TransformNode>(_tree.node<Node>(cur_node)._childrens[i]));
        }

        _transform_cache.groups.emplace_back(static_cast<uint32_t>(child_size));
    }

    //_tree.begin()->parent() != INVALID_HANDLE(Node)

    size_t const job_count = _tree.begin()->_childrens.size();

    for(uint32_t i = 0; i < job_count; ++i) {

        thread_pool.push(
            [&](TransformNode& node) {

                if(node.parent() != INVALID_HANDLE(Node)) {
                    node._model_global = _tree.node<TransformNode>(node.parent())._model_global * node._model_local;
                } else {
                    node._model_global = node._model_local;
                }
            },
            std::ref(_tree.node<TransformNode>(_tree.begin()->_childrens[i]))
        );
    }
}