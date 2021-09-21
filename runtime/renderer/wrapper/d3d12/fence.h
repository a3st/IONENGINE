// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

class D3DFence : public Fence {
public:

    D3DFence(ID3D12Device4* d3d12_device, const uint64 initial_value) {

        ASSERT_SUCCEEDED(d3d12_device->CreateFence(initial_value, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), m_d3d12_fence.put_void()));
        m_fence_event = CreateEvent(nullptr, false, false, nullptr);
        if(!m_fence_event) {
            ASSERT_SUCCEEDED(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    void wait(const uint64 value) override {
        if(m_d3d12_fence->GetCompletedValue() < value) {
            ASSERT_SUCCEEDED(m_d3d12_fence->SetEventOnCompletion(value, m_fence_event));
            WaitForSingleObjectEx(m_fence_event, INFINITE, false);
        }
    }

    uint64 get_completed_value() const override { return m_d3d12_fence->GetCompletedValue(); }
    void signal(const uint64 value) override { m_d3d12_fence->Signal(value); }

    ID3D12Fence* get_d3d12_fence() { return m_d3d12_fence.get(); }

private:

    winrt::com_ptr<ID3D12Fence> m_d3d12_fence;

    HANDLE m_fence_event;
};

}