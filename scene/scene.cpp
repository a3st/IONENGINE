// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene.h>

using namespace ionengine::scene;

void Scene::update_transform(lib::ThreadPool& thread_pool) {

    std::stack<std::pair<uint32_t, Handle<Node>>> stack;
    stack.emplace(0, _tree.root());

    _transform_cache.data.clear();
    _transform_cache.groups.resize(128);

    while(!stack.empty()) {

        std::pair<uint32_t, Handle<Node>> cur_node = stack.top();
        stack.pop();

        //std::cout << std::format("Get Node Level: {}", cur_node.first) << std::endl;

        size_t const child_size = _tree.node<Node>(cur_node.second)._childrens.size();

        for(size_t i = 0; i < child_size; ++i) {

            uint32_t const cur_node_index = cur_node.first + 1;

            stack.emplace(cur_node_index, _tree.node<Node>(cur_node.second)._childrens[i]);

        //    std::cout << "Node Level: " << cur_node_index << " Node Name: " << (char*)_tree.node<TransformNode>(_tree.node<Node>(cur_node.second)._childrens[i]).name().data() << std::endl;

            _transform_cache.data.emplace_back(&_tree.node<TransformNode>(_tree.node<Node>(cur_node.second)._childrens[i]));
            ++_transform_cache.groups[cur_node_index - 1];
        }

        //std::cout << "---------- End -----------" << std::endl;
    }

    uint32_t cur_group_index = 0;
    uint32_t last_data_index = 0;

    for(size_t i = 0; i < _transform_cache.groups.size(); ++i) {

        if(_transform_cache.groups[cur_group_index] == 0) {
            break;
        }

        uint32_t const transform_per_thread = std::max<uint32_t>(_transform_cache.groups[cur_group_index], _transform_cache.groups[cur_group_index] / 16);

        uint32_t group_job_count = std::max<uint32_t>(1, transform_per_thread);

        std::cout << "Next Job Group: " << group_job_count << std::endl;

        for(uint32_t j = last_data_index; j < last_data_index + group_job_count; ++j) {

            std::cout << "Pos: " << _transform_cache.data[j]->transform().position.x <<
                " (" << (char*)_transform_cache.data[j]->name().data() << ")" << std::endl;

            /*thread_pool.push(
                [&](TransformNode& node) {
                    std::cout << (char*)node.name().data() << std::endl;
                    std::cout << node.transform().position.x << std::endl;
                    //if(node.parent() != INVALID_HANDLE(Node)) {
                        //node._model_global = _tree.node<TransformNode>(node.parent())._model_global * node._model_local;
                    //} else {
                        //node._model_global = node._model_local;
                    //}
                },
                std::ref(*_transform_cache.data[j])
            );*/
        }

        last_data_index += group_job_count; 
        ++cur_group_index;

        thread_pool.wait_all();

        std::cout << "----------------------" << std::endl;
    }
}