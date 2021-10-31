// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class PipelineCache {

public:

    struct Key {

        uint32_t dummy;

        inline bool operator<(const Key& rhs) const {

            return false;
        }
    };

    PipelineCache(lgfx::Device* device);

    PipelineCache(const PipelineCache&) = delete;
    PipelineCache(PipelineCache&&) = delete;

    PipelineCache& operator=(const PipelineCache&) = delete;
    PipelineCache& operator=(PipelineCache&&) = delete;

    lgfx::Pipeline* GetPipeline(const Key& key);

    inline void Reset() {

        pipelines_.clear();
    }

private:

    lgfx::Device* device_;

    std::map<Key, std::unique_ptr<lgfx::Pipeline>> pipelines_;

};

}