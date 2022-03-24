// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>

namespace ionengine::renderer {

struct UploadInfo {
    Handle<Buffer> buffer;
    uint64_t offset;
    size_t size;
};

class UploadManager {
public:

    UploadManager(Backend& backend, size_t const buffer_size, uint32_t const buffer_count);

    bool request_upload_info(size_t const upload_size, UploadInfo& upload_info);

    void upload_data(
        Handle<Buffer> const& dest, 
        uint64_t const offset, 
        UploadInfo const& upload_info,
        std::span<char8_t const> const data
    );

    FenceResultInfo submit(Encoder& encoder);

    void reset();

private:

    Backend* _backend{nullptr};

    struct UploadStream {
        Handle<Buffer> buffer;
        uint64_t offset;
    };

    std::vector<UploadStream> _upload_streams;
    std::vector<Encoder> _copy_encoders;

    uint32_t _cur_upload_index{0};
    uint32_t _buffer_size{0};

    struct Upload

    std::queue<UploadInfo> _upload_infos;
};

}