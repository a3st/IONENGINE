// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <ui/render_interface.h>

using namespace ionengine;
using namespace ionengine::ui;

RenderInterface::RenderInterface()  {

}

void RenderInterface::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) {

}

void RenderInterface::EnableScissorRegion(bool enable) {

}

void RenderInterface::SetScissorRegion(int x, int y, int width, int height) {

}

bool RenderInterface::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) {

    

    return false;
}

bool RenderInterface::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) {

    //auto gpu_texture = GPUTexture::procedural(*_device, backend::Format::RGBA8_UNORM, source_dimensions.x, source_dimensions.y, 1, 1, backend::TextureFlags::ShaderResource | backend::TextureFlags::HostWrite);


    return false;
}

void RenderInterface::ReleaseTexture(Rml::TextureHandle texture_handle) {

    
}