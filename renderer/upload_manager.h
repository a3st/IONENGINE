// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>

namespace ionengine::renderer {

using UploadInfo = std::pair<Handle<Buffer>, uint64_t>;

class UploadManager {
public:

    UploadManager(Backend& backend, size_t const buffer_size, uint32_t const buffer_count);

    UploadInfo request_upload_info();

    void reset();

private:

    Backend* _backend{nullptr};

    std::vector<UploadInfo> _upload_infos;

    uint32_t _cur_upload_index{0};
};

}