// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DDescriptorSet : public DescriptorSet  {
public:

    D3DDescriptorSet(ID3D12Device4* d3d12_device, D3DDescriptorSetLayout* layout) : m_d3d12_device(d3d12_device), m_layout(layout) {
        
    }

    void update_descriptor(const uint32 slot, View* view) override {


    }

    void copy_descriptor(const uint32 src_slot, DescriptorSet* dest, const uint32 dst_slot) override {

        
    }

    void bind(ID3D12GraphicsCommandList4* command_list) {

        auto& descriptor_tables = m_layout->get_descriptor_tables();

		std::vector<ID3D12DescriptorHeap*> d3d12_descriptor_heaps;
		for(uint32 i = 0; i < descriptor_tables.size(); ++i) {
			d3d12_descriptor_heaps.emplace_back(m_descriptor_heaps[descriptor_tables[i].second.heap_type].get());
		}

		if(!d3d12_descriptor_heaps.empty()) {
			command_list->SetDescriptorHeaps(static_cast<uint32>(d3d12_descriptor_heaps.size()), d3d12_descriptor_heaps.data());
		}

		for(uint32 i = 0; i < descriptor_tables.size(); ++i) {
			
            D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = {
                m_descriptor_heaps[descriptor_tables[i].second.heap_type]->GetGPUDescriptorHandleForHeapStart().ptr + 
                    descriptor_tables[i].first * m_d3d12_device->GetDescriptorHandleIncrementSize(descriptor_tables[i].second.heap_type)
            };

			if(descriptor_tables[i].second.compute) {
				command_list->SetComputeRootDescriptorTable(i, gpu_handle);
			} else {
				command_list->SetGraphicsRootDescriptorTable(i, gpu_handle);
			}	
		}
    }

private:

    ID3D12Device4* m_d3d12_device;

    std::map<D3D12_DESCRIPTOR_HEAP_TYPE, winrt::com_ptr<ID3D12DescriptorHeap>> m_descriptor_heaps;

    D3DDescriptorSetLayout* m_layout;
};

}