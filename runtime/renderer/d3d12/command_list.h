// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DCommandList : public CommandList {
public:

    D3DCommandList(winrt::com_ptr<ID3D12Device4>& device, const CommandListType list_type) : m_device(device)  {
        
        ASSERT_SUCCEEDED(m_device.get()->CreateCommandAllocator(
            convert_command_list_type(list_type), 
            __uuidof(ID3D12CommandAllocator), m_d3d12_command_allocator.put_void()
        ));

        ASSERT_SUCCEEDED(m_device.get()->CreateCommandList(
            0, 
            convert_command_list_type(list_type), 
            m_d3d12_command_allocator.get(), 
            nullptr, 
            __uuidof(ID3D12GraphicsCommandList), m_d3d12_command_list.put_void()
        ));
    }

    void bind_pipeline(Pipeline& pipeline) override {

        m_pipeline = static_cast<D3DPipeline&>(pipeline);

        if(m_pipeline.value().get().get_type() == PipelineType::Graphics) {

            m_d3d12_command_list->SetGraphicsRootSignature(m_pipeline.value().get().get_root_signature().get());
            m_d3d12_command_list->SetPipelineState(m_pipeline.value().get().get_pipeline_state().get());
            m_d3d12_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        } else {

            
        }
    }

    void close() override {
        m_d3d12_command_list->Close();
    }

    void reset() override {
        m_d3d12_command_list->Reset(m_d3d12_command_allocator.get(), nullptr);
    }

    winrt::com_ptr<ID3D12CommandAllocator>& get_command_allocator() { return m_d3d12_command_allocator; }
    winrt::com_ptr<ID3D12GraphicsCommandList>& get_command_list() { return m_d3d12_command_list; }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    winrt::com_ptr<ID3D12CommandAllocator> m_d3d12_command_allocator;
    winrt::com_ptr<ID3D12GraphicsCommandList> m_d3d12_command_list;

    std::optional<std::reference_wrapper<D3DPipeline>> m_pipeline;
};

}