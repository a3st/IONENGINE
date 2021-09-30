// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class D3DDescriptorPool : public DescriptorPool {
public:

	D3DDescriptorPool(winrt::com_ptr<ID3D12Device4>& device, const std::vector<DescriptorPoolSize>& sizes) : m_d3d12_device(device) {
		
		std::map<D3D12_DESCRIPTOR_HEAP_TYPE, uint32> descriptor_counts;
		for(auto& size : sizes) {

			descriptor_counts[convert_descriptor_heap_type(size.type)] += size.count;
		}

		m_d3d12_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = nullptr;
		m_d3d12_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = nullptr;
		m_d3d12_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = nullptr;
		m_d3d12_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = nullptr;

		m_descriptor_offsets[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = 0;
		m_descriptor_offsets[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = 0;
		m_descriptor_offsets[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = 0;
		m_descriptor_offsets[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = 0;

		for(auto& descriptor : descriptor_counts) {

			D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
			heap_desc.Type = descriptor.first;
			heap_desc.NumDescriptors = descriptor.second;
			heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			if(heap_desc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV || heap_desc.Type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
				heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			}

			ASSERT_SUCCEEDED(m_d3d12_device.get()->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), m_d3d12_descriptor_heaps[descriptor.first].put_void()));
		}
	}

	D3D12_GPU_DESCRIPTOR_HANDLE get_d3d12_gpu_descriptor_handle(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type, const uint64 offset) {
		return { m_d3d12_descriptor_heaps[heap_type]->GetGPUDescriptorHandleForHeapStart().ptr + offset };
	}

	D3D12_CPU_DESCRIPTOR_HANDLE get_d3d12_cpu_descriptor_handle(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type, const uint64 offset) {
		return { m_d3d12_descriptor_heaps[heap_type]->GetCPUDescriptorHandleForHeapStart().ptr + offset };
	}

	winrt::com_ptr<ID3D12DescriptorHeap>& get_d3d12_descriptor_heap(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type) {
		return m_d3d12_descriptor_heaps[heap_type];
	}

	uint64 d3d12_allocate(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type) {
		m_descriptor_offsets[heap_type] += static_cast<uint64>(m_d3d12_device.get()->GetDescriptorHandleIncrementSize(heap_type));
		return m_descriptor_offsets[heap_type];
	}

	void d3d12_deallocate(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type) {
		m_descriptor_offsets[heap_type] -= static_cast<uint64>(m_d3d12_device.get()->GetDescriptorHandleIncrementSize(heap_type));
	}

private:

	std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_d3d12_device;

	std::map<D3D12_DESCRIPTOR_HEAP_TYPE, winrt::com_ptr<ID3D12DescriptorHeap>> m_d3d12_descriptor_heaps;
	std::map<D3D12_DESCRIPTOR_HEAP_TYPE, uint64> m_descriptor_offsets;
};

class D3DDescriptorSet : public DescriptorSet {
public:

	D3DDescriptorSet(winrt::com_ptr<ID3D12Device4>& device, D3DDescriptorPool& descriptor_pool, D3DDescriptorSetLayout& layout) : 
		m_d3d12_device(device), m_descriptor_pool(descriptor_pool), m_layout(layout) {
			
	}

	void bind(winrt::com_ptr<ID3D12GraphicsCommandList4>& command_list) {

		auto& descriptor_tables = m_layout.get().get_descriptor_tables();

		std::vector<ID3D12DescriptorHeap*> descriptor_heaps;
		for(uint32 i = 0; i < descriptor_tables.size(); ++i) {
			descriptor_heaps.emplace_back(m_descriptor_pool.get().get_d3d12_descriptor_heap(descriptor_tables[i].type).get());
		}

		if(!descriptor_heaps.empty()) {
			command_list->SetDescriptorHeaps(static_cast<uint32>(descriptor_heaps.size()), descriptor_heaps.data());
		}

		for(uint32 i = 0; i < descriptor_tables.size(); ++i) {
			
			if(descriptor_tables[i].compute) {
				command_list->SetComputeRootDescriptorTable(i, m_descriptor_pool.get().get_d3d12_gpu_descriptor_handle(descriptor_tables[i].type, descriptor_tables[i].offset));
			} else {
				command_list->SetGraphicsRootDescriptorTable(i, m_descriptor_pool.get().get_d3d12_gpu_descriptor_handle(descriptor_tables[i].type, descriptor_tables[i].offset));
			}	
		}
	}

private:

	std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_d3d12_device;
	std::reference_wrapper<D3DDescriptorSetLayout> m_layout;

	std::reference_wrapper<D3DDescriptorPool> m_descriptor_pool;
};

}