// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/gpu_texture.h>
#include <lib/math/color.h>
#include <lib/hash/crc32.h>

namespace ionengine::renderer {

struct AttachmentDesc {
    std::string name;
    backend::Format format;
    uint32_t width;
    uint32_t height;
    backend::TextureFlags flags;
};

class RenderPass;

class Attachment {

    friend class FrameGraph;

public:

    Attachment(AttachmentDesc const& attachment_desc) {

        _name = attachment_desc.name;
        _format = attachment_desc.format;
        _width = attachment_desc.width;
        _height = attachment_desc.height;
        _flags = attachment_desc.flags;
        _is_persistent = false;

        _hash = lib::hash::ctcrc32(_name.data(), _name.size());
    }

    Attachment(std::string_view const name, backend::MemoryState const before, backend::MemoryState const after) {

        _name = name;
        _before = before;
        _after = after;
        _is_persistent = true;

        _hash = lib::hash::ctcrc32(_name.data(), _name.size());
    }

    bool is_persistent() const { return _is_persistent; }

    backend::Handle<backend::Texture> attachment(uint32_t const frame_index) const { return _attachments.at(frame_index); }

    void attachment(uint32_t const frame_index, backend::Handle<backend::Texture> const& texture) { _attachments[frame_index] = texture; }

    uint32_t hash() const { return _hash; }

private:

    std::string _name;
    backend::Format _format{backend::Format::Unknown};
    uint32_t _width{0};
    uint32_t _height{0};
    backend::TextureFlags _flags;
    backend::MemoryState _before;
    backend::MemoryState _after;
    bool _is_persistent{false};
    std::vector<backend::Handle<backend::Texture>> _attachments;
    std::vector<RenderPass*> _render_passes;
    uint32_t _hash;
};

struct CreateColorInfo {
    std::shared_ptr<GPUTexture> attachment;
    backend::RenderPassLoadOp load_op;
    lib::math::Color clear_color;
};

struct CreateInputInfo {
    std::shared_ptr<GPUTexture> attachment;
};

struct CreateDepthStencilInfo {
    std::shared_ptr<GPUTexture> attachment;
    backend::RenderPassLoadOp load_op;
    float clear_depth;
    uint8_t clear_stencil;
};

class RenderPassContext {

    friend class FrameGraph;

public:

    RenderPassContext() = default;

    std::shared_ptr<GPUTexture> input(uint32_t const index) const { 
        return _inputs[index];
    }

    backend::Handle<backend::RenderPass> render_pass() const { 
        return _render_pass;
    }
    
    backend::Handle<backend::CommandList> command_list() const { 
        return _command_list;
    }

private:

    std::span<std::shared_ptr<GPUTexture>> _inputs;
    
    backend::Handle<backend::RenderPass> _render_pass;
    backend::Handle<backend::CommandList> _command_list;
};

using RenderPassFunc = std::function<void(RenderPassContext const&)>;
inline RenderPassFunc RenderPassDefaultFunc = [&](RenderPassContext const& context) { };

class FrameGraph {
public:

    FrameGraph(backend::Device& device);
    
    void add_pass(
        std::string_view const name, 
        uint32_t const width,
        uint32_t const height,
        std::optional<std::span<CreateColorInfo const>> const colors,
        std::optional<std::span<CreateInputInfo const>> const inputs,
        std::optional<CreateDepthStencilInfo> const depth_stencil,
        RenderPassFunc const& func
    );
    
    void reset();
    
    void execute();

    void wait();

private:

    struct RenderPass {
        std::string name;
        uint32_t width;
        uint32_t height;
        std::vector<std::shared_ptr<GPUTexture>> color_attachments;
        std::vector<backend::RenderPassLoadOp> color_ops;
        std::vector<lib::math::Color> color_clears;
        std::shared_ptr<GPUTexture> ds_attachment;
        backend::RenderPassLoadOp ds_op;
        std::pair<float, uint8_t> ds_clear;
        std::vector<std::shared_ptr<GPUTexture>> inputs;
        RenderPassFunc func;
        backend::Handle<backend::RenderPass> render_pass;
        backend::Handle<backend::CommandList> command_list;
    };

    std::vector<backend::Handle<backend::CommandList>> _command_lists;

    backend::Device* _device;

    std::unordered_map<uint32_t, RenderPass> _render_pass_cache;

    std::vector<RenderPass*> _render_passes;

    enum class OpType {
        RenderPass,
        ComputePass,
        Submit
    };

    struct OpData {
        OpType op_type;
        size_t index;
    };

    std::vector<OpData> _ops;

    uint32_t _frame_index{0};
    uint32_t _frame_count{2};

    std::vector<uint64_t> _fence_values;

    backend::Handle<backend::Texture> _swapchain_texture;
    backend::MemoryState _swapchain_memory_state;

    backend::Handle<backend::RenderPass> compile_render_pass(
        std::span<std::shared_ptr<GPUTexture>> color_attachments,
        std::span<backend::RenderPassLoadOp> color_ops,
        std::shared_ptr<GPUTexture> depth_stencil_attachment,
        backend::RenderPassLoadOp depth_stencil_op
    );
};

}
