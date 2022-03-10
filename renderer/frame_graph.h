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

struct RenderPassDesc {
    using AttachmentInfo = std::pair<uint32_t, RenderPassLoadOp>;

    std::string _name;
    std::array<Color, 8> _clear_colors;
    std::array<AttachmentInfo, 8> color_infos;
    uint32_t color_count{0};
    AttachmentInfo depth_stencil_info;
    float _clear_depth;
    uint8_t _clear_stencil;
    bool has_depth_stencil{false};
    std::vector<uint32_t> inputs;
    uint32_t width;
    uint32_t height;

    RenderPassDesc& name(std::string const& name) {
        
        _name = name;
        return *this;
    }

    RenderPassDesc& color(uint32_t const id, RenderPassLoadOp const load_op, Color const& clear_color) {
        
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

    RenderPassDesc& input(uint32_t const id) {
        
        inputs.emplace_back(id);
        return *this;
    }

    RenderPassDesc& depth_stencil(uint32_t const id, RenderPassLoadOp const load_op, float const clear_depth, uint8_t const clear_stencil) {
        
        _clear_depth = clear_depth;
        _clear_stencil = clear_stencil;
        depth_stencil_info = AttachmentInfo { id, load_op };
        has_depth_stencil = true;
        return *this;
    }

    bool operator<(RenderPassDesc const& other) const {
        
        return std::tie(color_infos, color_count, depth_stencil_info, has_depth_stencil) < std::tie(other.color_infos, other.color_count, other.depth_stencil_info, other.has_depth_stencil);
    }
};

class RenderPassResources {
public:

    RenderPassResources() = default;

    Handle<Texture> get(uint32_t const id) const { return _attachments.find(id)->second; }

private:

    friend class FrameGraph;

    std::unordered_map<uint32_t, Handle<Texture>> _attachments;
};

using RenderPassFunc = std::function<void(Handle<renderer::RenderPass> const&, RenderPassResources const&)>;

class FrameGraph {
public:

    FrameGraph() = default;

    FrameGraph& attachment(uint32_t const id, Format const format, uint32_t const width, uint32_t const height);
    FrameGraph& external_attachment(uint32_t const id, Format const format, MemoryState const before, MemoryState const after);
    FrameGraph& render_pass(uint32_t const id, RenderPassDesc const& desc, RenderPassFunc const& func);
    FrameGraph& bind_external_attachment(uint32_t const id, Handle<Texture> const& handle);

    void build(Backend& backend, uint32_t const flight_frames);
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
    using FrameId = std::pair<uint32_t, uint32_t>;

    std::unordered_map<FrameId, FrameGraph::RenderPass, pair_hash> _render_passes;
    std::unordered_map<FrameId, Attachment, pair_hash> _attachments;
    std::unordered_map<FrameId, RenderPassResources, pair_hash> _render_pass_resources;

    enum class OpType : uint32_t {
        RenderPass
    };

    using Op = std::pair<FrameGraph::OpType, uint32_t>;

    std::vector<Op> _ops;

    std::unordered_set<uint32_t> _external_render_passes_ids;
    std::unordered_map<uint32_t, uint32_t> _external_attachments_ids;

    uint32_t _flight_frame_index{0};
    uint32_t _flight_frames{0};

    Handle<renderer::RenderPass> create_render_pass(Backend& backend, RenderPassDesc const& desc);
};

}
