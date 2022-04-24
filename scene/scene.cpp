// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/scene.h>

using namespace ionengine::scene;

Scene::Scene() {

}

/*
void Scene::update_transform(lib::ThreadPool& thread_pool) {

    _transform_cache.nodes.resize(128);
    _transform_cache.groups.resize(128);

    _tree.dfs<TransformNode>(_transform_cache.groups, _transform_cache.nodes);

    uint32_t cur_group_index = 0;
    uint32_t last_data_index = 0;
    
    for(size_t i = 0; i < _transform_cache.groups.size(); ++i) {

        if(_transform_cache.groups[cur_group_index] == 0) {
            break;
        }

        uint32_t const transform_per_thread = std::max<uint32_t>(_transform_cache.groups[cur_group_index], _transform_cache.groups[cur_group_index] / 16);
        uint32_t group_job_count = std::max<uint32_t>(1, transform_per_thread);

        for(uint32_t j = last_data_index; j < last_data_index + group_job_count; ++j) {

            thread_pool.push(
                [&](TransformNode& node) {
                    if(node.parent() != _tree.root()) {
                        node._model_global = _tree.node<TransformNode>(node.parent())._model_global * node._model_local;
                    } else {
                        node._model_global = node._model_local;
                    }
                },
                std::ref(_tree.node<TransformNode>(_transform_cache.nodes[j]))
            );
        }

        last_data_index += group_job_count; 
        ++cur_group_index;

        thread_pool.wait_all();
    }
}
*/