

auto pipeline = _device->create_pipeline(
            shader.descriptor_layout, 
            shader.attributes, 
            shader.stages, 
            shader.rasterizer, 
            shader.depth_stencil, 
            shader.blend, 
            render_pass, 
            backend::InvalidHandle<backend::CachePipeline>()
        );