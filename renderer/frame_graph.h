// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/color.h>

namespace ionengine {

struct pair_hash {
    template<typename TypeFirst, typename TypeSecond> 
    size_t operator()(std::pair<TypeFirst, TypeSecond> const& other) const {
        return std::hash<TypeFirst>()(other.first) ^ std::hash<TypeSecond>()(other.second);
    }
};

}

namespace ionengine::renderer {

using AttachmentId = uint32_t;

struct RenderPassDesc {
    using AttachmentInfo = std::pair<AttachmentId, RenderPassLoadOp>;

    std::u8string name;
    std::array<Color, 8> clear_colors;
    std::array<AttachmentInfo, 8> color_infos;
    uint32_t color_count;
    AttachmentInfo depth_stencil_info;
    float clear_depth;
    uint8_t clear_stencil;
    bool has_depth_stencil;
    std::vector<AttachmentId> inputs;
    uint32_t width;
    uint32_t height;

    RenderPassDesc& set_name(std::u8string const& _name) {
        
        name = _name;
        return *this;
    }

    RenderPassDesc& set_color(AttachmentId const id, RenderPassLoadOp const load_op, Color const& clear_color) {
        
        clear_colors[color_count] = clear_color;
        color_infos[color_count] = AttachmentInfo { id, load_op };
        ++color_count;
        return *this;
    }

    RenderPassDesc& set_rect(uint32_t const _width, uint32_t const _height) {
        width = _width;
        height = _height;
        return *this;
    }

    RenderPassDesc& set_input(AttachmentId const id) {
        
        inputs.emplace_back(id);
        return *this;
    }

    RenderPassDesc& set_depth_stencil(AttachmentId const id, RenderPassLoadOp const load_op, float const _clear_depth, uint8_t const _clear_stencil) {
        
        clear_depth = _clear_depth;
        clear_stencil = _clear_stencil;
        depth_stencil_info = AttachmentInfo { id, load_op };
        has_depth_stencil = true;
        return *this;
    }
};

using BufferId = uint32_t;

struct ComputePassDesc {
    std::u8string name;
    // TODO!
};

class RenderPassContext {
public:

    RenderPassContext() = default;

    Handle<Texture> get_attachment(AttachmentId const id) const { return _attachments.find(id)->second; }

    Handle<renderer::RenderPass> render_pass() const { return _render_pass; }
    
    Encoder& encoder() const { return *_encoder; }

    uint32_t pass_index() const { return _pass_index; }

    uint32_t flight_frame_index() const { return _flight_frame_index; }

private:

    friend class FrameGraph;

    std::unordered_map<AttachmentId, Handle<Texture>> _attachments;
    
    Handle<renderer::RenderPass> _render_pass;
    Encoder* _encoder;
    uint32_t _pass_index;
    uint32_t _flight_frame_index;
};

class ComputePassContext {
public:

    ComputePassContext() = default;

    Handle<Buffer> get_buffer(BufferId const id) const { return _buffers.find(id)->second; }

private:

    friend class FrameGraph;

    std::unordered_map<BufferId, Handle<Buffer>> _buffers;
};

using RenderPassId = uint32_t;
using RenderPassFunc = std::function<void(RenderPassContext const&)>;
using ComputePassId = uint32_t;
using ComputePassFunc = std::function<void(ComputePassContext const&)>;

class FrameGraph {
public:

    FrameGraph() = default;

    FrameGraph& attachment(AttachmentId const id, Format const format, uint32_t const width, uint32_t const height, TextureFlags const flags);
    FrameGraph& external_attachment(AttachmentId const id, Format const format, MemoryState const before, MemoryState const after);
    FrameGraph& render_pass(RenderPassId const id, RenderPassDesc const& render_pass_desc, RenderPassFunc const& func);
    FrameGraph& compute_pass(ComputePassId const id, ComputePassDesc const& compute_pass_desc, ComputePassFunc const& func);
    FrameGraph& bind_external_attachment(uint32_t const id, Handle<Texture> const& target);

    void build(Backend& backend, uint32_t const flight_frame_count);
    void reset(Backend& backend);
    
    FenceResultInfo execute(Backend& backend, Encoder& encoder);

private:

    struct RenderPass {
        RenderPassDesc desc;
        Handle<renderer::RenderPass> render_pass;
        RenderPassFunc func;
        bool is_compiled;
    };

    struct InternalAttachment {
        Format format;
        uint32_t width;
        uint32_t height;
        TextureFlags flags;
        Handle<Texture> target;
    };

    struct ExternalAttachment {
        Format format;
        MemoryState before;
        MemoryState after;
        Handle<Texture> target;
    };

    using Attachment = std::variant<FrameGraph::InternalAttachment, FrameGraph::ExternalAttachment>;

    using RenderPassFrameId = std::pair<RenderPassId, uint32_t>;
    using AttachmentFrameId = std::pair<AttachmentId, uint32_t>;

    std::unordered_map<RenderPassFrameId, FrameGraph::RenderPass, pair_hash> _render_passes;
    std::unordered_map<AttachmentFrameId, FrameGraph::Attachment, pair_hash> _attachments;
    std::unordered_map<RenderPassFrameId, RenderPassContext, pair_hash> _render_pass_contexts;

    using MemoryStateInfo = std::pair<MemoryState, std::unordered_set<RenderPassId>>;

    std::unordered_map<AttachmentId, MemoryStateInfo> _memory_states;

    std::unordered_set<RenderPassId> _external_render_passes;
    std::unordered_map<AttachmentId, std::unordered_set<RenderPassId>> _external_attachments;

    enum class OpType : uint32_t {
        RenderPass,
        ComputePass
    };

    using Op = std::pair<FrameGraph::OpType, uint32_t>;

    std::vector<Op> _ops;

    uint32_t _flight_frame_index{0};
    uint32_t _flight_frame_count{0};

    Handle<renderer::RenderPass> create_render_pass(Backend& backend, RenderPassDesc const& desc);
};

}
