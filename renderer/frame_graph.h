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

    RenderPassDesc& name(std::string const& name_) {
        _name = name_;
        return *this;
    }

    RenderPassDesc& color(uint32_t const id, RenderPassLoadOp const load_op, Color const& clear_color) {
        _clear_colors[color_count] = clear_color;
        color_infos[color_count] = AttachmentInfo { id, load_op };
        ++color_count;
        return *this;
    }

    RenderPassDesc& depth_stencil(uint32_t const id, RenderPassLoadOp const load_op, float const clear_depth, uint8_t const clear_stencil) {
        _clear_depth = clear_depth;
        _clear_stencil = clear_stencil;
        depth_stencil_info = AttachmentInfo { id, load_op };
        return *this;
    }
};

class FrameGraph {
public:

    FrameGraph() = default;

    FrameGraph& attachment(uint32_t const id, Format const format, Extent2D extent);
    FrameGraph& external_attachment(uint32_t const id, Format const format, MemoryState const before, MemoryState const after);
    FrameGraph& render_pass(uint32_t const id, RenderPassDesc const& desc, std::function<void()> const& func);
    FrameGraph& bind_external_attachment(uint32_t const id, Handle<Texture> const& handle);

    void build(Backend& backend, uint32_t const flight_frames);
    void reset(Backend& backend);
    void execute(Backend& backend);

private:

    struct RenderPass {
        RenderPassDesc desc;
        Handle<renderer::RenderPass> render_pass;
        std::function<void()> func;
    };

    struct InternalAttachment {
        Format format;
        Extent2D extent;
        Handle<Texture> target;
    };

    struct ExternalAttachment {
        Format format;
        MemoryState before;
        MemoryState after;
        Handle<Texture> target;
    };

    using Attachment = std::variant<InternalAttachment, ExternalAttachment>;
    using FrameId = std::pair<uint32_t, uint32_t>;

    std::unordered_map<FrameId, RenderPass, pair_hash> _render_passes;
    std::unordered_map<FrameId, Attachment, pair_hash> _attachments;

    enum class OpType : uint32_t {
        RenderPass
    };

    using Op = std::pair<OpType, uint32_t>;

    std::vector<Op> _ops;

    std::unordered_set<uint32_t> _external_render_passes_ids;
    std::unordered_map<uint32_t, uint32_t> _external_attachments_ids;

    uint32_t _flight_frame_index{0};
    uint32_t _flight_frames{0};

    Handle<renderer::RenderPass> create_render_pass(Backend& backend, RenderPass const& desc);
};

}
