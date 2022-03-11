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

    std::u8string _name;
    std::array<Color, 8> _clear_colors;
    std::array<AttachmentInfo, 8> color_infos;
    uint32_t color_count{0};
    AttachmentInfo depth_stencil_info;
    float _clear_depth;
    uint8_t _clear_stencil;
    bool has_depth_stencil{false};
    std::vector<AttachmentId> inputs;
    uint32_t width;
    uint32_t height;

    RenderPassDesc& name(std::u8string const& name) {
        
        _name = name;
        return *this;
    }

    RenderPassDesc& color(AttachmentId const id, RenderPassLoadOp const load_op, Color const& clear_color) {
        
        _clear_colors[color_count] = clear_color;
        color_infos[color_count] = AttachmentInfo { id, load_op };
        ++color_count;
        return *this;
    }

    RenderPassDesc& rect(uint32_t const _width, uint32_t const _height) {
        width = _width;
        height = _height;
        return *this;
    }

    RenderPassDesc& input(AttachmentId const id) {
        
        inputs.emplace_back(id);
        return *this;
    }

    RenderPassDesc& depth_stencil(AttachmentId const id, RenderPassLoadOp const load_op, float const clear_depth, uint8_t const clear_stencil) {
        
        _clear_depth = clear_depth;
        _clear_stencil = clear_stencil;
        depth_stencil_info = AttachmentInfo { id, load_op };
        has_depth_stencil = true;
        return *this;
    }

    bool operator<(RenderPassDesc const& other) const {
        
        return std::tie(color_infos, color_count, depth_stencil_info, has_depth_stencil) < 
            std::tie(other.color_infos, other.color_count, other.depth_stencil_info, other.has_depth_stencil);
    }
};

struct ComputePassDesc {

    std::u8string _name;

};

class RenderPassResources {
public:

    RenderPassResources() = default;

    Handle<Texture> get(uint32_t const id) const { return _attachments.find(id)->second; }

private:

    friend class FrameGraph;

    std::unordered_map<uint32_t, Handle<Texture>> _attachments;
};

using RenderPassId = uint32_t;
using RenderPassFunc = std::function<void(Handle<renderer::RenderPass> const&, RenderPassResources const&)>;
using ComputePassId = uint32_t;
using ComputePassFunc = std::function<void(RenderPassResources const&)>;

class FrameGraph {
public:

    FrameGraph() = default;

    FrameGraph& attachment(AttachmentId const id, Format const format, uint32_t const width, uint32_t const height);
    FrameGraph& external_attachment(uint32_t const id, Format const format, MemoryState const before, MemoryState const after);
    FrameGraph& render_pass(RenderPassId const id, RenderPassDesc const& desc, RenderPassFunc const& func);
    FrameGraph& compute_pass(ComputePassId const id, ComputePassDesc const& desc, ComputePassFunc const& func);
    FrameGraph& bind_external_attachment(uint32_t const id, Handle<Texture> const& target);

    void build(Backend& backend, uint32_t const flight_frame_count);
    void reset(Backend& backend);
    void execute(Backend& backend);

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
    std::unordered_map<AttachmentFrameId, Attachment, pair_hash> _attachments;
    std::unordered_map<RenderPassFrameId, RenderPassResources, pair_hash> _render_pass_resources;

    std::unordered_map<AttachmentId, std::unordered_set<RenderPassId>> _memory_states;

    std::unordered_set<RenderPassId> _external_render_passes;
    std::unordered_map<AttachmentId, RenderPassId> _external_attachments;

    enum class OpType : uint32_t {
        RenderPass
    };

    using Op = std::pair<FrameGraph::OpType, uint32_t>;

    std::vector<Op> _ops;

    uint32_t _flight_frame_index{0};
    uint32_t _flight_frame_count{0};

    Handle<renderer::RenderPass> create_render_pass(Backend& backend, RenderPassDesc const& desc);
};

}
