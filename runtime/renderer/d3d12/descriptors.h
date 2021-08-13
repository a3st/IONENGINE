// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

struct DescriptorTableDesc {
	D3D12_DESCRIPTOR_HEAP_TYPE type;
	uint32 count;
	uint32 offset;
	bool compute;
};

class D3DDescriptorSetLayout : public DescriptorSetLayout {
public:

    D3DDescriptorSetLayout(winrt::com_ptr<ID3D12Device4>& device, const std::vector<DescriptorSetLayoutBinding>& bindings) : m_device(device) {

		using Key = std::pair<D3D12_DESCRIPTOR_HEAP_TYPE, ShaderType>;
		std::map<Key, std::vector<D3D12_DESCRIPTOR_RANGE>> ranges_by_key;

		for(auto& binding : bindings) {
			
			D3D12_DESCRIPTOR_RANGE range{};
			range.RangeType = convert_descriptor_range_type(binding.view_type);
			range.NumDescriptors = binding.count;
			range.BaseShaderRegister = binding.slot;
			range.RegisterSpace = binding.space;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			ranges_by_key[{ convert_descriptor_heap_type(binding.view_type), binding.shader_type }].emplace_back(range);
		}

		std::vector<D3D12_ROOT_PARAMETER> parameters;

		uint32 offset = 0;

		for(auto& range : ranges_by_key) {
			
			D3D12_ROOT_PARAMETER parameter{};
			parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameter.DescriptorTable.NumDescriptorRanges = static_cast<uint32>(range.second.size());
			parameter.DescriptorTable.pDescriptorRanges = range.second.data();
			parameter.ShaderVisibility = convert_shader_visibility(range.first.second);

			parameters.emplace_back(parameter);

			DescriptorTableDesc table_desc{};
			table_desc.type = range.first.first;
			table_desc.count = static_cast<uint32>(range.second.size());
			table_desc.offset = offset;

			m_descriptor_tables.emplace_back(table_desc);
			offset = m_device.get()->GetDescriptorHandleIncrementSize(range.first.first) * table_desc.count;
		}

        D3D12_ROOT_SIGNATURE_DESC root_desc{};
	    root_desc.NumParameters = static_cast<uint32>(parameters.size());
	    root_desc.pParameters = parameters.data();
	    root_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	    winrt::com_ptr<ID3DBlob> blob;
	    ASSERT_SUCCEEDED(D3D12SerializeRootSignature(&root_desc, D3D_ROOT_SIGNATURE_VERSION_1_0, blob.put(), nullptr));
	    ASSERT_SUCCEEDED(m_device.get()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), __uuidof(ID3D12RootSignature), m_d3d12_root_signature.put_void()));
    }

	winrt::com_ptr<ID3D12RootSignature>& get_root_signature() { return m_d3d12_root_signature; }
	const std::vector<DescriptorTableDesc>& get_descriptor_tables() { return m_descriptor_tables; }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    winrt::com_ptr<ID3D12RootSignature> m_d3d12_root_signature;

	std::vector<DescriptorTableDesc> m_descriptor_tables;
};

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

	D3D12_GPU_DESCRIPTOR_HANDLE get_gpu_descriptor_handle(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type, const uint64 offset) {
		return { m_d3d12_descriptor_heaps[heap_type]->GetGPUDescriptorHandleForHeapStart().ptr + offset };
	}

	D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_descriptor_handle(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type, const uint64 offset) {
		return { m_d3d12_descriptor_heaps[heap_type]->GetCPUDescriptorHandleForHeapStart().ptr + offset };
	}

	winrt::com_ptr<ID3D12DescriptorHeap>& get_descriptor_heap(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type) {
		return m_d3d12_descriptor_heaps[heap_type];
	}

	uint64 allocate(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type) {
		m_descriptor_offsets[heap_type] += static_cast<uint64>(m_d3d12_device.get()->GetDescriptorHandleIncrementSize(heap_type));
		return m_descriptor_offsets[heap_type];
	}

	void deallocate(const D3D12_DESCRIPTOR_HEAP_TYPE heap_type) {
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
			descriptor_heaps.emplace_back(m_descriptor_pool.get().get_descriptor_heap(descriptor_tables[i].type).get());
		}

		if(!descriptor_heaps.empty()) {
			command_list->SetDescriptorHeaps(static_cast<uint32>(descriptor_heaps.size()), descriptor_heaps.data());
		}

		for(uint32 i = 0; i < descriptor_tables.size(); ++i) {
			
			if(descriptor_tables[i].compute) {
				command_list->SetComputeRootDescriptorTable(i, m_descriptor_pool.get().get_gpu_descriptor_handle(descriptor_tables[i].type, descriptor_tables[i].offset));
			} else {
				command_list->SetGraphicsRootDescriptorTable(i, m_descriptor_pool.get().get_gpu_descriptor_handle(descriptor_tables[i].type, descriptor_tables[i].offset));
			}	
		}
	}

private:

	std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_d3d12_device;
	std::reference_wrapper<D3DDescriptorSetLayout> m_layout;

	std::reference_wrapper<D3DDescriptorPool> m_descriptor_pool;
};

}