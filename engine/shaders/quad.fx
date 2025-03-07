// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Quad";
    Description = "Output texture as a quad";
    Domain = "Screen";
}

DATA {
    texture2D_t inputTexture;
}

VS {
    VS_OUTPUT main(VS_INPUT input) {
        VS_OUTPUT output;
        output.uv = float2((input.id << 1) & 2, input.id & 2);
        output.position = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
        return output;
    }
}

[FillMode("SOLID"), CullMode("BACK"), DepthWrite(true), StencilWrite(false)]
PS {
    PS_OUTPUT main(VS_OUTPUT input) {
        cbuffer_t<SAMPLER_DATA> samplerData = gShaderData.samplerData;
        cbuffer_t<EFFECT_DATA> effectData = gShaderData.effectData;

        PS_OUTPUT output;
        output.color = effectData.Get().inputTexture.Get().Sample(samplerData.Get().linearSampler.Get(), input.uv);
        return output;
    }
}