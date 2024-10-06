// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Quad";
    Description = "Quad Shader";
    Domain = "Screen";
}

DATA {
    texture2D_t inputTexture;
}

VS {
    #include "shared/common.hlsli"

    VS_OUTPUT main(VS_INPUT input) {
        VS_OUTPUT output;
        output.uv = float2((input.id << 1) & 2, input.id & 2);
        output.position = float4(output.uv * 2.0 + -1.0, 0.0, 1.0);
        return output;
    }
}

PS {
    #include "shared/common.hlsli"

    PS_OUTPUT main(VS_OUTPUT input) {
        //Texture2D inputTexture = GetTexture(MATERIAL_DATA, materialBuffer, inputTexture);
        //SamplerState linearSampler = GetSampler();

        // cbuffer_t<SAMPLER_DATA> b = gShaderData.samplerBuffer;

        PS_OUTPUT output;
        output.color = float4(0.5f, 1.0f, 1.0f, 1.0f);
        //output.color = inputTexture.Sample(linearSampler, input.uv);
        return output;
    }
}

OUTPUT {
    CullSide = "BACK";
    DepthWrite = false;
    StencilWrite = false;
}