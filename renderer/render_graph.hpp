// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer_pool.hpp"
#include "render_target.hpp"
#include <xxhash/xxhash64.h>

namespace ionengine::renderer
{
    inline uint32_t constexpr RG_RESOURCE_DEFAULT_LIFETIME = 1;

    class RGResourcePool : public core::ref_counted_object
    {
      public:
        struct Entry
        {
            rhi::TextureFormat format;
            uint32_t sample_count;
            uint32_t width;
            uint32_t height;
            rhi::TextureUsageFlags flags;

            auto operator==(Entry const& other) const -> bool
            {
                return std::tie(format, sample_count, width, height, flags) ==
                       std::tie(other.format, other.sample_count, other.width, other.height, flags);
            }
        };

        struct EntryHasher
        {
            auto operator()(const Entry& entry) const -> std::size_t
            {
                return XXHash64::hash(&entry.format, sizeof(rhi::TextureFormat), 0) ^
                       XXHash64::hash(&entry.sample_count, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.width, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.height, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.flags, sizeof(uint32_t), 0);
            }
        };

        RGResourcePool(rhi::Device& device);

        auto get(rhi::TextureFormat const format, uint32_t const sample_count, uint32_t const width,
                 uint32_t const height, rhi::TextureUsageFlags const flags, uint64_t const hash)
            -> core::ref_ptr<rhi::Texture>;

        auto update() -> void;

        auto reset() -> void;

      private:
        rhi::Device* device;
        core::ref_ptr<rhi::MemoryAllocator> allocator;

        using TimedResource = std::pair<core::ref_ptr<rhi::Texture>, uint64_t>;

        struct Chunk
        {
            std::vector<TimedResource> resources;
            std::vector<uint32_t> free;
            uint32_t offset;
        };
        std::unordered_map<Entry, std::unique_ptr<Chunk>, EntryHasher> entries;

        struct ResourceAllocation
        {
            Chunk* chunk;
            uint32_t offset;
        };
        std::unordered_map<uint64_t, ResourceAllocation> allocations;
    };

    enum class RGAttachmentUsage
    {
        RenderTarget,
        DepthStencil
    };

    struct RGColorAttachment
    {
        rhi::RenderPassLoadOp load_op;
        rhi::RenderPassStoreOp store_op;
        math::Color clear_color;
    };

    struct RGDepthStencilAttachment
    {
        rhi::RenderPassLoadOp depth_load_op;
        rhi::RenderPassStoreOp depth_store_op;
        rhi::RenderPassLoadOp stencil_load_op;
        rhi::RenderPassStoreOp stencil_store_op;
        float clear_depth;
        uint8_t clear_stencil;
    };

    struct RGAttachmentInfo
    {
        std::string attachment_name;
        std::variant<RGColorAttachment, RGDepthStencilAttachment> data;
    };

    struct RGAttachment
    {
        std::string name;
        uint32_t width;
        uint32_t height;
        RGAttachmentUsage usage;
        core::ref_ptr<RenderTarget> render_target;
    };

    using RGGraphicsAttachment = std::pair<uint64_t, std::variant<RGColorAttachment, RGDepthStencilAttachment>>;

    class RGRenderPassContext
    {
      public:
        RGRenderPassContext(std::vector<uint64_t>& inputs, std::vector<RGGraphicsAttachment>& outputs,
                            std::unordered_map<uint64_t, RGAttachment>& attachments, RGResourcePool& resource_pool,
                            BufferPool& buffer_pool, rhi::CommandBuffer& command_buffer, uint32_t const frame_index)
            : inputs(&inputs), outputs(&outputs), attachments(&attachments), resource_pool(&resource_pool),
              buffer_pool(&buffer_pool), command_buffer(&command_buffer), frame_index(frame_index)
        {
        }

        auto get_command_buffer() -> rhi::CommandBuffer&
        {
            return *command_buffer;
        }

        template <typename Type>
        auto bind_buffer(std::string_view const binding, Type const& data, rhi::BufferUsage const usage) -> void
        {
            auto buffer =
                buffer_pool->allocate(sizeof(Type), (rhi::BufferUsageFlags)(rhi::BufferUsage::MapWrite | usage),
                                      std::span<uint8_t const>((uint8_t*)&data, sizeof(Type)));

            command_buffer->bind_descriptor(binding, rhi::BufferBindData{buffer, usage});
        }

        auto bind_texture(std::string_view const binding, core::ref_ptr<rhi::Texture> texture,
                          rhi::TextureUsage const usage) -> void
        {
            command_buffer->bind_descriptor(binding, rhi::TextureBindData{texture, usage});
        }

        auto bind_attachment_as(uint32_t const index, std::string_view const binding) -> void
        {
            auto result = attachments->find(inputs->at(index));
            assert(result != attachments->end());

            core::ref_ptr<rhi::Texture> texture = nullptr;

            rhi::TextureUsageFlags flags = 0;
            rhi::TextureFormat format = rhi::TextureFormat::Unknown;

            if (result->second.render_target)
            {
                texture = result->second.render_target->get_buffer(frame_index);
            }
            else
            {
                if (result->second.usage == RGAttachmentUsage::RenderTarget)
                {
                    flags =
                        (rhi::TextureUsageFlags)(rhi::TextureUsage::RenderTarget | rhi::TextureUsage::ShaderResource);
                    format = rhi::TextureFormat::RGBA8_UNORM;
                }
                else
                {
                    flags = (rhi::TextureUsageFlags)(rhi::TextureUsage::DepthStencil);
                    format = rhi::TextureFormat::D32_FLOAT;
                }

                texture =
                    resource_pool->get(format, 1, result->second.width, result->second.height, flags, result->first);
            }

            if (binding.empty())
            {
                command_buffer->bind_descriptor(result->second.name,
                                                rhi::TextureBindData{texture, rhi::TextureUsage::ShaderResource});
            }
            else
            {
                command_buffer->bind_descriptor(binding,
                                                rhi::TextureBindData{texture, rhi::TextureUsage::ShaderResource});
            }
        }

      private:
        std::vector<uint64_t>* inputs;
        std::vector<RGGraphicsAttachment>* outputs;
        std::unordered_map<uint64_t, RGAttachment>* attachments;
        RGResourcePool* resource_pool;
        BufferPool* buffer_pool;
        rhi::CommandBuffer* command_buffer;
        uint32_t frame_index;
    };

    using graphics_pass_func_t = std::function<void(RGRenderPassContext&)>;

    class RGRenderPass
    {
      public:
        RGRenderPass(std::string_view const pass_name) : pass_name(pass_name)
        {
        }

        auto get_name() -> std::string_view
        {
            return pass_name;
        }

        virtual auto setup(rhi::Device& device, std::unordered_map<uint64_t, RGAttachment>& attachments,
                           RGResourcePool& resource_pool, BufferPool& buffer_pool,
                           core::ref_ptr<rhi::Texture> swapchain, uint32_t const frame_index) -> void = 0;

      private:
        std::string pass_name;
    };

    class RGGraphicsRenderPass : public RGRenderPass
    {
      public:
        RGGraphicsRenderPass(std::string_view const pass_name, uint32_t const width, uint32_t const height,
                             std::vector<uint64_t> const& inputs, std::vector<RGGraphicsAttachment> const& outputs,
                             graphics_pass_func_t callback);

        auto setup(rhi::Device& device, std::unordered_map<uint64_t, RGAttachment>& attachments,
                   RGResourcePool& resource_pool, BufferPool& buffer_pool, core::ref_ptr<rhi::Texture> swapchain,
                   uint32_t const frame_index) -> void override;

      private:
        std::vector<uint64_t> inputs;
        std::vector<RGGraphicsAttachment> outputs;
        uint32_t width;
        uint32_t height;
        graphics_pass_func_t callback;
    };

    class RenderGraph : public core::ref_counted_object
    {
      public:
        RenderGraph(rhi::Device& device, std::vector<std::unique_ptr<RGRenderPass>>&& render_passes,
                    std::unordered_map<uint64_t, RGAttachment>&& attachments);

        auto execute() -> void;

        inline uint64_t static SWAPCHAIN_RESERVED_HASH;

      private:
        rhi::Device* device;

        struct FrameInfo
        {
            core::ref_ptr<BufferPool> buffer_pool;
            core::ref_ptr<RGResourcePool> resource_pool;
        };
        std::vector<FrameInfo> frame_infos;

        std::vector<std::unique_ptr<RGRenderPass>> render_passes;
        std::unordered_map<uint64_t, RGAttachment> attachments;
        uint32_t frame_index;
    };

    class RenderGraphBuilder
    {
      public:
        RenderGraphBuilder() = default;

        auto add_attachment(std::string_view const attachment_name, uint32_t const width, uint32_t const height,
                            RGAttachmentUsage const usage, core::ref_ptr<RenderTarget> render_target = nullptr)
            -> RenderGraphBuilder&;

        auto add_graphics_pass(std::string_view const pass_name, uint32_t const width, uint32_t const height,
                               std::vector<std::string> const& inputs, std::vector<RGAttachmentInfo> const& outputs,
                               graphics_pass_func_t const& callback) -> RenderGraphBuilder&;

        auto build(rhi::Device& device) -> core::ref_ptr<RenderGraph>;

      private:
        std::vector<std::unique_ptr<RGRenderPass>> render_passes;
        std::unordered_map<uint64_t, RGAttachment> attachments;
    };
} // namespace ionengine::renderer