// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene.h>

using namespace ionengine::scene;

void Scene::update_transform(lib::ThreadPool& thread_pool) {

    std::stack<Handle<Node>> stack;

    Handle<Node> cur_node = _tree.root();

    while(cur_node != INVALID_HANDLE(Node)) {

        size_t const child_size = _tree.node<Node>(cur_node)._childrens.size();

        for(size_t i = 0; i < child_size; ++i) {

            stack.emplace(_tree.node<Node>(cur_node)._childrens[i]);

            std::cout << (char*)_tree.node<TransformNode>(_tree.node<Node>(cur_node)._childrens[i]).name().data() << std::endl;

            _transform_cache.data.emplace_back(&_tree.node<TransformNode>(_tree.node<Node>(cur_node)._childrens[i]));
        }

        if(!stack.empty()) {

            cur_node = stack.top();
            stack.pop();

            _transform_cache.groups.emplace_back(static_cast<uint32_t>(child_size));

            std::cout << child_size << std::endl;
        } else {
            cur_node = INVALID_HANDLE(Node);
        }

        //break;
    }

    for(uint32_t i = 0; i < 1; ++i) {

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