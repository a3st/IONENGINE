// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<>
class BindingSet<backend::d3d12>  {
public:

    BindingSet(ID3D12Device4* d3d12_device, BindingSetLayout<backend::d3d12>* layout) : m_d3d12_device(d3d12_device), m_layout(layout) {

        assert(d3d12_device && "pointer to d3d12_device is null");
        assert(layout && "pointer to layout is null");
        
        std::map<D3D12_DESCRIPTOR_HEAP_TYPE, uint32> descriptor_sizes;
        descriptor_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = 0;
        descriptor_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = 0;

        auto& descriptor_tables = m_layout->get_descriptor_tables();
        for(auto& descriptor_table : descriptor_tables) {
            descriptor_sizes[descriptor_table.second.heap_type] += descriptor_table.second.count;
        }

        if(descriptor_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] > 0) {
            auto d3d12_heap = create_heap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptor_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]);
            m_descriptor_heaps.emplace(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, d3d12_heap);
        }

        if(descriptor_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] > 0) {
            auto d3d12_heap = create_heap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, descriptor_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER]);
            m_descriptor_heaps.emplace(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, d3d12_heap);
        }
    }

    void write(const WriteBindingSet<backend::d3d12>& write_binding_set) {

        auto& descriptor_tables = m_layout->get_descriptor_tables();

        for(uint32 i = 0; i < write_binding_set.count; ++i) {

            D3D12_DESCRIPTOR_HEAP_TYPE heap_type = gfx_to_d3d12_descriptor_heap_type(write_binding_set.views[i]->get_type());
            
            D3D12_CPU_DESCRIPTOR_HANDLE src_handle = {
                write_binding_set.views[i]->get_descriptor_ptr().heap->d3d12_heap->GetCPUDescriptorHandleForHeapStart().ptr +
                    write_binding_set.views[i]->get_descriptor_ptr().offset * m_d3d12_device->GetDescriptorHandleIncrementSize(heap_type)
            };

            D3D12_CPU_DESCRIPTOR_HANDLE dst_handle = {
                m_descriptor_heaps[heap_type]->GetCPUDescriptorHandleForHeapStart().ptr +
                    descriptor_tables[write_binding_set.slot].first * m_d3d12_device->GetDescriptorHandleIncrementSize(heap_type) 
            };

            m_d3d12_device->CopyDescriptorsSimple(descriptor_tables[write_binding_set.slot].second.count, dst_handle, src_handle, heap_type);
        }
    }

    void set_descriptor_tables(ID3D12GraphicsCommandList4* command_list) {

        auto& descriptor_tables = m_layout->get_descriptor_tables();

		std::vector<ID3D12DescriptorHeap*> d3d12_descriptor_heaps;
		for(auto& descriptor_table : descriptor_tables) {
			d3d12_descriptor_heaps.emplace_back(m_descriptor_heaps[descriptor_table.second.heap_type].get());
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

    winrt::com_ptr<ID3D12DescriptorHeap> create_heap(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type, const uint32 descriptor_count) {
        winrt::com_ptr<ID3D12DescriptorHeap> d3d12_heap;

        D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
        heap_desc.Type = heap_type;
        heap_desc.NumDescriptors = descriptor_count;
        heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        THROW_IF_FAILED(m_d3d12_device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), d3d12_heap.put_void()));
        return d3d12_heap;
    }

    BindingSetLayout<backend::d3d12>* m_layout;

    std::map<D3D12_DESCRIPTOR_HEAP_TYPE, winrt::com_ptr<ID3D12DescriptorHeap>> m_descriptor_heaps;
};

}