// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
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

struct AttachmentDesc {
    std::string name;
    backend::Format format;
    uint32_t width;
    uint32_t height;
    backend::TextureFlags flags;
};

class Attachment {

    friend class FrameGraph;

public:

    Attachment(AttachmentDesc const& attachment_desc) {

        _name = attachment_desc.name;
        _format = attachment_desc.format;
        _width = attachment_desc.width;
        _height = attachment_desc.height;
    }

    Attachment(std::string_view const name, backend::MemoryState const before, backend::MemoryState const after) {

        _name = name;
        _before = before;
        _after = after;
    }

    backend::Handle<backend::Texture> attachment(uint32_t const index) const;

    bool is_persistent() const { return _is_persistent; }

private:

    std::string _name;
    backend::Format _format{backend::Format::Unknown};
    uint32_t _width{0};
    uint32_t _height{0};
    backend::TextureFlags _flags;
    backend::MemoryState _before;
    backend::MemoryState _after;
    std::vector<backend::Handle<backend::Texture>> _attachments;
    bool _is_persistent{false};
};

struct CreateColorInfo {
    Attachment* attachment;
    backend::RenderPassLoadOp load_op;
    Color clear_color;
};

struct CreateDepthStencilInfo {
    Attachment* attachment;
    backend::RenderPassLoadOp load_op;
    float clear_depth;
    uint8_t clear_stencil;
};

struct RenderPassDesc {
    std::string name;
    uint32_t width;
    uint32_t height;
    std::optional<std::span<CreateColorInfo const>> color_infos;
    std::optional<CreateDepthStencilInfo> depth_stencil_info;
    std::optional<std::span<Attachment const>> inputs;

    RenderPassDesc& set_name(std::string const& _name) {

        name = _name;
        return *this;
    }

    RenderPassDesc& set_rect(uint32_t const _width, uint32_t const _height) {

        width = _width;
        height = _height;
        return *this;
    }

    RenderPassDesc& set_color_infos(std::span<CreateColorInfo const> const _color_infos) {
    
        color_infos = _color_infos;
        return *this;
    }

    RenderPassDesc& set_depth_stencil(CreateDepthStencilInfo const& _depth_stencil_info) {
        
        depth_stencil_info = _depth_stencil_info;
        return *this;
    }

    RenderPassDesc& set_inputs(std::span<Attachment const> const _inputs) {
        
        inputs = _inputs;
        return *this;
    }
};

class RenderPassContext {
public:

    RenderPassContext() = default;

    backend::Handle<backend::Texture> attachment(uint32_t const index) { return _attachments[index].attachment(_frame_index); }

    backend::Handle<backend::RenderPass> render_pass() const { return _render_pass; }
    
    backend::Handle<backend::CommandList> command_list() const { return _command_list; }

    uint32_t pass_index() const { return _pass_index; }

    uint32_t frame_index() const { return _frame_index; }

private:

    friend class FrameGraph;

    std::span<Attachment const> _attachments;
    
    backend::Handle<backend::RenderPass> _render_pass;
    backend::Handle<backend::CommandList> _command_list;

    uint32_t _pass_index{0};
    uint32_t _frame_index{0};
};

using RenderPassFunc = std::function<void(RenderPassContext const&)>;

class RenderPass {

    friend class FrameGraph;

public:

    RenderPass(RenderPassDesc const& render_pass_desc, RenderPassFunc const& func) {

        _name = render_pass_desc.name;
        _width = render_pass_desc.width;
        _height = render_pass_desc.height;
        _color_infos = render_pass_desc.color_infos;
        _depth_stencil_info = render_pass_desc.depth_stencil_info;
        _inputs = render_pass_desc.inputs;
        _func = func;
    }

    void operator()(RenderPassContext const& context) { _func(context); }

    bool is_compiled() const { return _is_compiled; }

    backend::Handle<backend::RenderPass> render_pass(uint32_t const frame_index) const { return _render_passes.at(frame_index); }

    void render_pass(uint32_t const frame_index, backend::Handle<backend::RenderPass> const& render_pass) { _render_passes[frame_index] = render_pass; }

private:

    std::string _name;
    uint32_t _width{0};
    uint32_t _height{0};
    std::optional<std::span<CreateColorInfo const>> _color_infos{std::nullopt};
    std::optional<CreateDepthStencilInfo> _depth_stencil_info{std::nullopt};
    std::optional<std::span<Attachment const>> _inputs{std::nullopt};
    RenderPassFunc _func;
    std::vector<backend::Handle<backend::RenderPass>> _render_passes;
    bool _is_compiled{false};
};

/*
struct ComputePassDesc {
    std::string name;
    // TODO!
};

class ComputePassContext {
public:

    ComputePassContext() = default;

    Handle<backend::Buffer> buffer(BufferId const id) const { return _buffers.find(id)->second; }

private:

    friend class FrameGraph;

    std::unordered_map<BufferId, backend::Handle<backend::Buffer>> _buffers;
};

using ComputePassFunc = std::function<void(ComputePassContext const&)>;
*/

class FrameGraph {
public:

    FrameGraph() = default;

    Attachment& add_attachment(AttachmentDesc const& attachment_desc);
    
    Attachment& add_attachment(std::string_view const name, backend::MemoryState const before, backend::MemoryState const after);
    
    RenderPass& add_pass(RenderPassDesc const& render_pass_desc, RenderPassFunc const& func);
    
    // Pass& add_pass(ComputePassDesc const& compute_pass_desc, ComputePassFunc const& func);
    
    void bind_attachment(Attachment& attachment, backend::Handle<backend::Texture> const& target);

    void build(backend::Device& device, uint32_t const frame_count);
    
    void reset(backend::Device& device);
    
    uint64_t execute(backend::Device& device, backend::Handle<backend::CommandList> const& command_list);

private:

    struct MemoryBarrier {
        backend::MemoryState state;
        std::unordered_set<uint32_t> switch_index;
    };

    std::vector<std::unique_ptr<Attachment>> _attachments;
    std::vector<std::unique_ptr<RenderPass>> _render_passes;

    std::unordered_map<uint32_t, MemoryBarrier> _attachment_barriers;
    std::unordered_map<uint32_t, MemoryBarrier> _buffer_barriers;

    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> _external_attachments;
    std::unordered_set<uint32_t> _external_passes;

    enum class OpType : uint8_t {
        RenderPass,
        ComputePass
    };

    struct OpData {
        OpType op_type;
        uint32_t index;
    };

    std::vector<OpData> _ops;

    uint32_t _frame_index{0};
    uint32_t _frame_count{0};

    // Handle<backend::RenderPass> create_render_pass(backend::Backend& backend, uint32_t const frame_index, RenderPassDesc const& desc);
};

}
