// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DDescriptorSet : public DescriptorSet  {
public:

    D3DDescriptorSet(D3DDescriptorSetLayout* layout) : m_layout(layout) {
        
    }

    void bind(ID3D12GraphicsCommandList4* command_list) {


    }

private:

    D3DDescriptorSetLayout* m_layout;
};

}