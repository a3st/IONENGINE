// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class D3DCommandQueue : public CommandQueue {
public:

    D3DCommandQueue(ID3D12Device4* device, const D3D12_COMMAND_LIST_TYPE list_type) {

        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        queue_desc.Type = list_type;
        ASSERT_SUCCEEDED(m_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_d3d12_queue.put_void()));
    }

    void wait(Fence& fence, const uint64 value) override {
        ASSERT_SUCCEEDED(m_d3d12_command_queue->Wait(static_cast<D3DFence&>(fence).get_d3d12_fence().get(), value));
    }

    void signal(Fence& fence, const uint64 value) override {
        ASSERT_SUCCEEDED(m_d3d12_command_queue->Signal(static_cast<D3DFence&>(fence).get_d3d12_fence().get(), value));
    }

    void execute_command_lists(const std::vector<std::reference_wrapper<CommandList>>& command_lists) override {
        std::vector<ID3D12CommandList*> command_list_ptrs;
        for(auto command : command_lists) {
			command_list_ptrs.emplace_back(static_cast<D3DCommandList&>(command.get()).get_d3d12_command_list().get());
        }
        m_d3d12_command_queue->ExecuteCommandLists(static_cast<uint32>(command_list_ptrs.size()), command_list_ptrs.data());
    }

    ID3D12CommandQueue* get_d3d12_queue() { return m_d3d12_queue.get(); }

private:

    winrt::com_ptr<ID3D12CommandQueue> m_d3d12_queue;
};

}