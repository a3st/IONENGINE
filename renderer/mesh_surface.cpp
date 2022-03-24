// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/mesh_surface.h>

using namespace ionengine::renderer;

bool MeshSurface::load_from_data(Backend& backend, MeshSurfaceData const& mesh_surface_data) {

    size_t const vertex_size = 
        sizeof(float) * mesh_surface_data.positions.size() + 
        sizeof(float) * mesh_surface_data.uv_normals.size()
    ;

    /*_vertices_buffer = backend.create_buffer(vertex_size, BufferFlags::VertexBuffer);

    UploadInfo upload_info;
    
    if(!upload_manager.request_upload_info(vertex_size, upload_info)) {
        return false;
    }
    upload_manager.upload_data(_vertices_buffer, 0, upload_info);

    size_t index_size = 0;

    for(auto& material_info : mesh_surface_data.material_infos) {
        
        index_size += index_size + material_info.indices.size();
    }

    if(!upload_manager.request_upload_info(index_size, upload_info)) {
        return false;
    }
    upload_manager.upload_data(_indices_buffer, 0, upload_info);*/
    return true;
}