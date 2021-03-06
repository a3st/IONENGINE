// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <scene/mesh_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine;
using namespace ionengine::scene;

MeshNode::MeshNode() {

}

void MeshNode::accept(SceneVisitor& visitor) {

    visitor(*this);
}

void MeshNode::mesh(asset::AssetPtr<asset::Mesh> mesh) {

    if(mesh->is_pending()) {
        mesh->wait();
    }
    _mesh = mesh;
    
    _override_materials.resize(_mesh->get().materials_size());
    for(uint32_t i = 0; i < _mesh->get().materials_size(); ++i) {
        _override_materials.at(i) = _mesh->get().material(i);
    }
}

asset::AssetPtr<asset::Mesh> MeshNode::mesh() {
    return _mesh;
}

asset::AssetPtr<asset::Material> MeshNode::material(uint32_t const index) {
    return _override_materials.at(index);
}

void MeshNode::material(uint32_t const index, asset::AssetPtr<asset::Material> material) {
    if(material->is_pending()) {
        material->wait();
    }
    _override_materials.at(index) = material;
}
