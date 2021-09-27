// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Texture {
public:

    Texture() {
        
    }

private:

    std::string m_path;

    wrapper::Buffer* m_buffer;
    wrapper::Sampler* m_sampler;
    wrapper::View* m_view;
};

}