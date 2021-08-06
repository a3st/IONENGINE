// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DCommandQueue : public CommandQueue {
public:

    D3DCommandQueue(winrt::com_ptr<ID3D12Device4>& device, const CommandListType type) : m_device(device) {

        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        queue_desc.Type = convert_enum(type);

        m_device.get()->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_d3d12_command_queue.put_void());
    }

    winrt::com_ptr<ID3D12CommandQueue>& get_command_queue() { return m_d3d12_command_queue; }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    winrt::com_ptr<ID3D12CommandQueue> m_d3d12_command_queue;
};

}