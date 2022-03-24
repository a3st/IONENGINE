// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/upload_manager.h>

using namespace ionengine::renderer;

UploadManager::UploadManager(Backend& backend, size_t const buffer_size, uint32_t const buffer_count) 
    : _backend(&backend), _buffer_size(buffer_size) {

    _upload_streams.resize(buffer_count);

    for(uint32_t i = 0; i < buffer_count; ++i) {
        
        Handle<Buffer> buffer = backend.create_buffer(buffer_size, BufferFlags::HostWrite);
        _upload_streams[i] = UploadStream { std::move(buffer), 0 };
    }
}

bool UploadManager::request_upload_info(size_t const upload_size, UploadInfo& upload_info) {

    bool result = false;

    auto& upload_stream = _upload_streams[_cur_upload_index]; 

    if(upload_stream.offset + upload_size <= _buffer_size) {

        auto upload_info = UploadInfo {};
        upload_info.buffer = upload_stream.buffer;
        upload_info.offset = upload_stream.offset;
        upload_info.size = upload_size;

        upload_stream.offset += upload_size;
        result = true;
    }

    return result;
}

void UploadManager::upload_data(
        Handle<Buffer> const& dest, 
        uint64_t const offset, 
        UploadInfo const& upload_info,
        std::span<char8_t const> const data
) {

    _backend->upload_buffer_data(
        upload_info.buffer,
        upload_info.offset,
        data
    );

    _upload_infos.emplace(upload_info);
}

FenceResultInfo UploadManager::submit(Encoder& encoder) {

    while(!_upload_infos.empty()) {

        UploadInfo upload_info = _upload_infos.front();
        _upload_infos.pop();

        _copy_encoders[_cur_upload_index].barrier(
            dest,
            MemoryState::Common,
            MemoryState::CopyDest
        );

        _copy_encoders[_cur_upload_index].copy_buffer_region(
            dest,
            offset,
            upload_info.buffer,
            upload_info.offset,
            data.size()
        );

        _copy_encoders[_cur_upload_index].barrier(
            dest,
            MemoryState::CopyDest,
            MemoryState::Common
        );
    }

    
    return {};
}

void UploadManager::reset() {

    _cur_upload_index = (_cur_upload_index + 1) % static_cast<uint32_t>(_upload_streams.size());
}
