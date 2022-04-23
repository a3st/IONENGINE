// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <asset/model.h>

namespace ionengine::renderer::frontend {

class GeometryBuffer {
public:

    GeometryBuffer(Context& context, asset::Model const& model, BufferUsage const usage);

    ~GeometryBuffer();

    void bind(backend::Handle<backend::CommandList> const& command_list);


private:

    Context* _context;
    
    backend::Handle<backend::Buffer> _buffer;
};

}
