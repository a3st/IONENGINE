// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/upload_manager.h>

using namespace ionengine::renderer;

UploadManager::UploadManager(Backend& backend, size_t const buffer_size, uint32_t const buffer_count) : _backend(&backend) {

    _upload_infos.resize(buffer_count);

    for(uint32_t i = 0; i < buffer_count; ++i) {
        
        Handle<Buffer> buffer = backend.create_buffer(buffer_size, BufferFlags::HostWrite);
        _upload_infos[i] = { std::move(buffer), 0 };
    }
}

UploadInfo UploadManager::request_upload_info() {

    return {};
}

void UploadManager::reset() {

    _upload_infos[_cur_upload_index].second = 0;
    _cur_upload_index = (_cur_upload_index + 1) % static_cast<uint32_t>(_upload_infos.size());
}
