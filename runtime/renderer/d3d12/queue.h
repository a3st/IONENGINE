// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DCommandQueue : public CommandQueue {
public:

    D3DCommandQueue(const ComPtr<ID3D12Device4>& device, const CommandListType type) {

        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        
        switch(type) {
            case CommandListType::Graphics: queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; break;
            case CommandListType::Copy: queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY; break;
            case CommandListType::Compute: queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE; break;
        }

        device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_d3d12_command_queue));
    }

    const ComPtr<ID3D12CommandQueue>& get_command_queue() const { return m_d3d12_command_queue; }

private:

    ComPtr<ID3D12CommandQueue> m_d3d12_command_queue;
};

}