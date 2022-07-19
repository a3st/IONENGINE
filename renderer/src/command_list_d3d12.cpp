// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>

#include <renderer/impl/command_list_d3d12.hpp>
#include <renderer/impl/device_d3d12.hpp>

using namespace ionengine;
using namespace ionengine::renderer;

D3D12_COMMAND_LIST_TYPE constexpr ionengine::renderer::to_command_list(
    CommandListType const list_type) {
    switch (list_type) {
        case CommandListType::Direct:
            return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case CommandListType::Copy:
            return D3D12_COMMAND_LIST_TYPE_COPY;
        case CommandListType::Compute:
            return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        default:
            throw std::invalid_argument("Invalid argument");
    }
}

void CommandList_D3D12::dispatch(uint32_t const thread_group_x,
                                 uint32_t const thread_group_y,
                                 uint32_t const thread_group_z) {
    _command_list->Dispatch(thread_group_x, thread_group_y, thread_group_z);
}

void CommandList_D3D12::begin() {}

void CommandList_D3D12::close() {
    HRESULT result = _command_list->Close();
    assert(result == S_OK && to_string(result).c_str());
}

void CommandList_D3D12::begin_render_pass(
    RenderPass& render_pass, std::span<math::Color const> const clear_colors,
    float const clear_depth, uint8_t const clear_stencil) {}

void CommandList_D3D12::end_render_pass() { _command_list->EndRenderPass(); }

void CommandList_D3D12::set_viewport(int32_t const x, int32_t const y,
                                     uint32_t const width,
                                     uint32_t const height) {
    D3D12_VIEWPORT viewport = {.TopLeftX = static_cast<float>(x),
                               .TopLeftY = static_cast<float>(y),
                               .Width = static_cast<float>(width),
                               .Height = static_cast<float>(height),
                               .MinDepth = D3D12_MIN_DEPTH,
                               .MaxDepth = D3D12_MAX_DEPTH};

    _command_list->RSSetViewports(1, &viewport);
}

void CommandList_D3D12::set_scissor(int32_t const left, int32_t const top,
                                    int32_t const right, int32_t const bottom) {
    D3D12_RECT rect = {.left = static_cast<LONG>(left),
                       .top = static_cast<LONG>(top),
                       .right = static_cast<LONG>(right),
                       .bottom = static_cast<LONG>(bottom)};

    _command_list->RSSetScissorRects(1, &rect);
}

void CommandList_D3D12::bind_pipeline(Pipeline& pipeline) {}

void CommandList_D3D12::copy_buffer_region(Buffer& dest, uint64_t const dest_offset,
                        Buffer& source, uint64_t const source_offset,
                        size_t const size) {
    //_command_list->CopyBufferRegion();
}

void CommandList_D3D12::copy_texture_region(
    Texture& dest, Buffer& source,
    std::span<TextureCopyRegionDesc const> const regions) {}

core::Expected<std::unique_ptr<CommandList>, std::string>
CommandList_D3D12::create(Device_D3D12& device, CommandListType const list_type,
                          bool const bundled) noexcept {
    auto command_list = std::make_unique<CommandList_D3D12>();

    HRESULT result;

    // Initialize class members
    result = device._device->CreateCommandList1(
        0, to_command_list(list_type), D3D12_COMMAND_LIST_FLAG_NONE,
        __uuidof(ID3D12GraphicsCommandList4),
        reinterpret_cast<void**>(command_list->_command_list.GetAddressOf()));

    if (result != S_OK) {
        return core::make_expected<std::unique_ptr<CommandList>, std::string>(
            to_string(result));
    }

    return core::make_expected<std::unique_ptr<CommandList>, std::string>(
        std::move(command_list));
}

core::Expected<std::unique_ptr<CommandList>, std::string> CommandList::create(
    Device& device, CommandListType const list_type,
    bool const bundled) noexcept {
    return CommandList_D3D12::create(static_cast<Device_D3D12&>(device),
                                     list_type, bundled);
}