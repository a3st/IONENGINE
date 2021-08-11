// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DCommandQueue : public CommandQueue {
public:

    D3DCommandQueue(winrt::com_ptr<ID3D12Device4>& device, const CommandListType type) : m_device(device) {

        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        queue_desc.Type = convert_command_list_type(type);

        ASSERT_SUCCEEDED(m_device.get()->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_d3d12_command_queue.put_void()));
    }

    void wait(Fence& fence, const uint64 value) override {
        ASSERT_SUCCEEDED(m_d3d12_command_queue->Wait(static_cast<D3DFence&>(fence).get_fence().get(), value));
    }

    void signal(Fence& fence, const uint64 value) override {
        ASSERT_SUCCEEDED(m_d3d12_command_queue->Signal(static_cast<D3DFence&>(fence).get_fence().get(), value));
    }

    void execute_command_lists(const std::vector<std::reference_wrapper<CommandList>>& command_lists) override {
        std::vector<ID3D12CommandList*> command_list_ptrs;
        for(auto command : command_lists) {
			command_list_ptrs.emplace_back(static_cast<D3DCommandList&>(command.get()).get_command_list().get());
        }
    }

    winrt::com_ptr<ID3D12CommandQueue>& get_command_queue() { return m_d3d12_command_queue; }
    winrt::com_ptr<ID3D12Device4>& get_device() { return m_device; }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    winrt::com_ptr<ID3D12CommandQueue> m_d3d12_command_queue;
};

}