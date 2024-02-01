// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "render_graph.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

RGResourcePool::RGResourcePool(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator) : device(&device), allocator(allocator) { 

}

auto RGResourcePool::get(
    rhi::TextureFormat const format, 
    uint32_t const sample_count, 
    uint32_t const width, 
    uint32_t const height,
    rhi::TextureUsageFlags const flags,
    uint64_t const hash
) -> core::ref_ptr<rhi::Texture> 
{
    auto result = allocations.find(hash);

    if(result != allocations.end()) {
        ResourceAllocation& allocation = result->second;
        allocation.chunk->resources[allocation.offset].second++;
        return allocation.chunk->resources[allocation.offset].first;
    } else {
        auto const key = Entry { format, sample_count, width, height, flags };
        auto entry = entries.find(key);

        if(entry != entries.end()) {
            core::ref_ptr<rhi::Texture> texture = nullptr;
            uint32_t offset = 0;

            if(static_cast<uint32_t>(entry->second->resources.size()) < entry->second->offset) {    
                texture = device->create_texture(
                    allocator, 
                    width, 
                    height, 
                    1, 
                    1, 
                    format, 
                    rhi::TextureDimension::_2D, 
                    flags,
                    {}
                ).get();

                auto chunk = entry->second.get();
                offset = chunk->offset;
                chunk->resources.emplace_back(std::make_pair(texture, RG_RESOURCE_DEFAULT_LIFETIME));
                chunk->free.emplace_back(0);
                chunk->offset++;
            }

            if(!texture) {
                auto chunk = entry->second.get();

                for(size_t i = chunk->offset; i < chunk->resources.size(); ++i) {
                    if(chunk->free[i] == 1) {
                        offset = chunk->offset;
                        texture = chunk->resources[i].first;
                        chunk->resources[i].second = RG_RESOURCE_DEFAULT_LIFETIME;
                        chunk->offset = i + 1;
                        break;
                    }
                }
            }

            auto allocation = ResourceAllocation {
                .chunk = entry->second.get(),
                .offset = offset
            };
            allocations.emplace(hash, allocation);
            return texture;
        } else {
            std::vector<TimedResource> resources;
            std::vector<uint32_t> free;

            auto texture = device->create_texture(
                allocator, 
                width, 
                height, 
                1, 
                1, 
                format, 
                rhi::TextureDimension::_2D, 
                flags,
                {}
            ).get();

            resources.emplace_back(std::make_pair(texture, RG_RESOURCE_DEFAULT_LIFETIME));
            free.emplace_back(0);

            auto chunk = std::make_unique<Chunk>(resources, free, 0);

            auto allocation = ResourceAllocation {
                .chunk = chunk.get(),
                .offset = 0
            };
            allocations.emplace(hash, allocation);

            entries.emplace(key, std::move(chunk));
            return texture;
        }
    }
}

auto RGResourcePool::update() -> void {

    for(auto it = allocations.begin(); it != allocations.end(); ) {
        auto chunk = it->second.chunk;
        uint32_t const offset = it->second.offset;

        if(chunk->resources[offset].second <= 0) {
            chunk->resources[offset].second = 0;
            chunk->free[offset] = 1;
            chunk->offset = offset;

            it = allocations.erase(it);
        } else {
            chunk->resources[offset].second--;
            it++;
        }
    }
}

auto RGResourcePool::reset() -> void {

    for(auto& [entry, chunk] : entries) {
        std::ranges::fill(chunk->free, 1);
        chunk->offset = 0;
    }
    allocations.clear();
}

RGGraphicsRenderPass::RGGraphicsRenderPass(
    std::string_view const pass_name,
    uint32_t const width,
    uint32_t const height,
    std::vector<uint64_t> const& inputs,
    std::vector<RGGraphicsAttachment> const& outputs,
    graphics_pass_func_t callback
) :
    RGRenderPass(pass_name),
    inputs(inputs),
    outputs(outputs),
    width(width),
    height(height),
    callback(callback)
{

}

auto RGGraphicsRenderPass::setup(
    rhi::Device& device,
    std::unordered_map<uint64_t, RGAttachment>& attachments,
    RGResourcePool& resource_pool,
    BufferPool& buffer_pool,
    core::ref_ptr<rhi::Texture> swapchain,
    uint32_t const frame_index
) -> void 
{
    auto command_buffer = device.allocate_command_buffer(rhi::CommandBufferType::Graphics);

    std::array<rhi::RenderPassColorInfo, 8> colors;
    std::optional<rhi::RenderPassDepthStencilInfo> depth_stencil;
    uint32_t offset = 0;
    
    for(auto& output : outputs) {
        if(output.first == RenderGraph::SWAPCHAIN_RESERVED_HASH) {
            assert(output.second.index() == 0 && "RenderTarget attachment cannot contain RGDepthStencilAttachment");
            auto color_attachment = std::get<RGColorAttachment>(output.second);

            colors[offset].load_op = color_attachment.load_op;
            colors[offset].store_op = color_attachment.store_op;
            colors[offset].clear_color = color_attachment.clear_color;
            colors[offset].texture = swapchain;

            offset++;
            continue;
        }

        auto result = attachments.find(output.first);
        assert(result != attachments.end());

        if(result->second.usage == RGAttachmentUsage::RenderTarget) {
            assert(output.second.index() == 0 && "RenderTarget attachment cannot contain RGDepthStencilAttachment");
            auto color_attachment = std::get<RGColorAttachment>(output.second);

            colors[offset].load_op = color_attachment.load_op;
            colors[offset].store_op = color_attachment.store_op;
            colors[offset].clear_color = color_attachment.clear_color;

            if(result->second.render_target) {
                colors[offset].texture = result->second.render_target->get_buffer(frame_index);
            } else {
                colors[offset].texture = resource_pool.get(
                    rhi::TextureFormat::RGBA8_UNORM,
                    1,
                    result->second.width,
                    result->second.height,
                    (rhi::TextureUsageFlags)(rhi::TextureUsage::RenderTarget | rhi::TextureUsage::ShaderResource),
                    result->first
                );
            }
            offset++;
        } else {
            assert(output.second.index() == 1 && "DepthStencil attachment cannot contain RGColorAttachment");
            auto depth_stencil_attachment = std::get<RGDepthStencilAttachment>(output.second);

            depth_stencil = rhi::RenderPassDepthStencilInfo {
                .depth_load_op = depth_stencil_attachment.depth_load_op,
                .depth_store_op = depth_stencil_attachment.depth_store_op,
                .stencil_load_op = depth_stencil_attachment.stencil_load_op,
                .stencil_store_op = depth_stencil_attachment.stencil_store_op,
                .clear_depth = depth_stencil_attachment.clear_depth,
                .clear_stencil = depth_stencil_attachment.clear_stencil
            };

            if(result->second.render_target) {
                depth_stencil.value().texture = result->second.render_target->get_buffer(frame_index);
            } else {
                depth_stencil.value().texture = resource_pool.get(
                    rhi::TextureFormat::D32_FLOAT,
                    1,
                    result->second.width,
                    result->second.height,
                    (rhi::TextureUsageFlags)(rhi::TextureUsage::DepthStencil),
                    result->first
                );
            }
        }
    }

    command_buffer->set_viewport(0, 0, width, height);
    command_buffer->set_scissor(0, 0, width, height);
    command_buffer->begin_render_pass(std::span<rhi::RenderPassColorInfo const>(colors.data(), offset), depth_stencil);
    RGRenderPassContext ctx(
        inputs,
        outputs,
        attachments,
        resource_pool,
        buffer_pool,
        &command_buffer,
        frame_index
    );
    callback(ctx);
    command_buffer->end_render_pass();

    command_buffer->close();

    std::array<core::ref_ptr<rhi::CommandBuffer>, 1> submits = {
        command_buffer
    };

    device.submit_command_lists(submits);
}

RenderGraph::RenderGraph(
    rhi::Device& device, 
    core::ref_ptr<rhi::MemoryAllocator> allocator,
    std::vector<std::unique_ptr<RGRenderPass>>&& render_passes,
    std::unordered_map<uint64_t, RGAttachment>&& attachments
) : 
    device(&device), 
    render_passes(std::move(render_passes)),
    attachments(std::move(attachments)),
    frame_index(0)
{
    for(uint32_t i = 0; i < 2; ++i) {
        auto frame_info = FrameInfo {
            .buffer_pool = core::make_ref<BufferPool>(device),
            .resource_pool = core::make_ref<RGResourcePool>(device, allocator)
        };
        frame_infos.emplace_back(std::move(frame_info));
    }

    std::string_view const reserve_string = "Swapchain";
    SWAPCHAIN_RESERVED_HASH = XXHash64::hash(reserve_string.data(), reserve_string.size(), 0);
}

auto RenderGraph::execute() -> void {

    auto swapchain_buffer = device->request_next_swapchain_buffer();
    frame_infos[frame_index].buffer_pool->reset();
    frame_infos[frame_index].resource_pool->reset();

    for(auto& render_pass : render_passes) {
        render_pass->setup(
            *device, 
            attachments,
            &frame_infos[frame_index].resource_pool,
            &frame_infos[frame_index].buffer_pool,
            swapchain_buffer,
            frame_index
        );
        frame_infos[frame_index].resource_pool->update();
    }
    
    device->present();
    frame_index = (frame_index + 1) % 2;
}

auto RenderGraphBuilder::add_attachment(
    std::string_view const attachment_name,
    uint32_t const width,
    uint32_t const height,
    RGAttachmentUsage const usage,
    core::ref_ptr<RenderTarget> render_target
) -> RenderGraphBuilder& 
{   
    uint64_t const hash = XXHash64::hash(attachment_name.data(), attachment_name.size(), 0);
    auto attachment = RGAttachment {
        .name = std::string(attachment_name),
        .width = width,
        .height = height,
        .usage = usage,
        .render_target = render_target
    };
    attachments.emplace(hash, std::move(attachment));
    return *this;
}

auto RenderGraphBuilder::add_graphics_pass(
    std::string_view const pass_name,
    uint32_t const width,
    uint32_t const height,
    std::vector<std::string> const& inputs,
    std::vector<RGAttachmentInfo> const& outputs,
    graphics_pass_func_t const& callback
) -> RenderGraphBuilder& 
{
    std::vector<uint64_t> input_attachments;
    for(size_t i = 0; i < inputs.size(); ++i) {
        uint64_t const hash = XXHash64::hash(inputs[i].data(), inputs[i].size(), 0);
        input_attachments.emplace_back(hash);
    }

    std::vector<RGGraphicsAttachment> output_attachments;
    for(size_t i = 0; i < outputs.size(); ++i) {
        uint64_t const hash = XXHash64::hash(outputs[i].attachment_name.data(), outputs[i].attachment_name.size(), 0);
        auto attachment = std::make_pair(hash, outputs[i].data);
        output_attachments.emplace_back(attachment);
    }

    auto render_pass = std::make_unique<RGGraphicsRenderPass>(pass_name, width, height, input_attachments, output_attachments, callback);
    render_passes.emplace_back(std::move(render_pass));
    return *this;
}

auto RenderGraphBuilder::build(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator) -> core::ref_ptr<RenderGraph> {

    return core::make_ref<RenderGraph>(device, allocator, std::move(render_passes), std::move(attachments));
}