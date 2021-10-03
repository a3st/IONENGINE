// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::rendersystem {

class PipelineCache {
public:

    struct Key {
        std::string name;

        bool operator<(const Key& rhs) const {
            return std::tie(name) < std::tie(rhs.name);
        }
    };

    PipelineCache(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");
    }

    gfx::Pipeline* get_pipeline(const std::string& name) {

        Key key = {
            name
        };

        auto it = m_pipelines.find(key);
        if(it != m_pipelines.end()) {
            return it->second.get();
        } else {

            return it->second.get();
        }
    }

    void clear() {
        m_pipelines.clear();
    }

private:

    gfx::Device* m_device;

    std::map<PipelineCache::Key, std::unique_ptr<gfx::Pipeline>> m_pipelines;
};

}