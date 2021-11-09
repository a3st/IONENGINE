// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "descriptor_layout.h"
#include "device.h"
#include "conversion.h"

using namespace lgfx;

DescriptorLayout::DescriptorLayout(Device* const device, const std::span<const DescriptorLayoutBinding> bindings) {

    std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
    ranges.resize(bindings.size());
    std::vector<D3D12_ROOT_PARAMETER> parameters;
    parameters.resize(bindings.size());

    for(size_t i : std::views::iota(0u, bindings.size())) {
		D3D12_DESCRIPTOR_RANGE range{};
		range.RangeType = ToD3D12DescriptorRangeType(bindings[i].type);
		range.NumDescriptors = bindings[i].count;
		range.BaseShaderRegister = bindings[i].slot;
		range.RegisterSpace = bindings[i].space;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        ranges[i] = range;

        descriptor_tables_.emplace_back(DescriptorTable { bindings[i].type, bindings[i].count, bindings[i].space });

        D3D12_ROOT_PARAMETER parameter{};
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameter.DescriptorTable.pDescriptorRanges = &ranges[i];
        parameter.DescriptorTable.NumDescriptorRanges = 1;
        parameter.ShaderVisibility = ToD3D12ShaderVisiblity(bindings[i].shader_visible);
		parameters[i] = parameter;
	}

    D3D12_ROOT_SIGNATURE_DESC root_desc{};
    root_desc.pParameters = parameters.data();
	root_desc.NumParameters = static_cast<uint32_t>(parameters.size());
	root_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> blob;
	THROW_IF_FAILED(D3D12SerializeRootSignature(&root_desc, D3D_ROOT_SIGNATURE_VERSION_1_0, blob.GetAddressOf(), nullptr));
	THROW_IF_FAILED(device->device_->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), __uuidof(ID3D12RootSignature), reinterpret_cast<void**>(root_signature_.GetAddressOf())));
}