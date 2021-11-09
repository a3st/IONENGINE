// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "descriptor_set.h"
#include "texture_view.h"
#include "buffer_view.h"
#include "device.h"
#include "descriptor_layout.h"

using namespace lgfx;

DescriptorSet::DescriptorSet(Device* const device, DescriptorLayout* const layout) : 
    device_(device),
    layout_(layout),
    srv_pool_(device, kDescriptorSetSRVCount, lgfx::DescriptorType::kShaderResource, lgfx::DescriptorFlags::kShaderVisible),
    sampler_pool_(device, kDescriptorSetSamplerCount, lgfx::DescriptorType::kSampler, lgfx::DescriptorFlags::kShaderVisible) {

    for(size_t i : std::views::iota(0u, layout_->descriptor_tables_.size())) {
        if(layout_->descriptor_tables_[i].type == DescriptorType::kShaderResource || layout_->descriptor_tables_[i].type == DescriptorType::kUnorderedAccess || layout_->descriptor_tables_[i].type == DescriptorType::kConstantBuffer) {
            for(size_t j : std::views::iota(0u, layout_->descriptor_tables_[i].count)) {
                DescriptorAllocInfo alloc_info = srv_pool_.Allocate();
                descriptors_[layout_->descriptor_tables_[i].slot].emplace_back(Key { static_cast<uint32_t>(i), layout_->descriptor_tables_[i].type, alloc_info });
            }
        } else {
            for(size_t j : std::views::iota(0u, layout_->descriptor_tables_[i].count)) {
                DescriptorAllocInfo alloc_info = sampler_pool_.Allocate();
                descriptors_[layout_->descriptor_tables_[i].slot].emplace_back(Key { static_cast<uint32_t>(i), layout_->descriptor_tables_[i].type, alloc_info });
            }
        }
    }
}

void DescriptorSet::WriteTexture(const uint32_t slot, const uint32_t space, TextureView* texture_view) {

    D3D12_CPU_DESCRIPTOR_HANDLE dst_handle = { descriptors_[space][slot].alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + descriptors_[space][slot].alloc_info.offset * device_->srv_descriptor_offset_ };
    D3D12_CPU_DESCRIPTOR_HANDLE src_handle = { texture_view->alloc_info_.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + texture_view->alloc_info_.offset * device_->srv_descriptor_offset_ };
    device_->device_->CopyDescriptorsSimple(1, dst_handle, src_handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    update_descriptors_.emplace_back(descriptors_[space][slot]);
}

void DescriptorSet::WriteBuffer(const uint32_t slot, const uint32_t space, BufferView* buffer_view) {

    D3D12_CPU_DESCRIPTOR_HANDLE dst_handle = { descriptors_[space][slot].alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + descriptors_[space][slot].alloc_info.offset * device_->srv_descriptor_offset_ };
    D3D12_CPU_DESCRIPTOR_HANDLE src_handle = { buffer_view->alloc_info_.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + buffer_view->alloc_info_.offset * device_->srv_descriptor_offset_ };
    device_->device_->CopyDescriptorsSimple(1, dst_handle, src_handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    update_descriptors_.emplace_back(descriptors_[space][slot]);
}