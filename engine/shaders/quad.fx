// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Quad";
    Description = "Output texture as a quad";
    Domain = "Screen";
}

DATA {
    uint inputTexture;
}

VS {
    VS_OUTPUT main(VS_INPUT input) 
    {
        VS_OUTPUT output;
        output.uv = float2((input.id << 1) & 2, input.id & 2);
        output.position = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
        return output;
    }
}

[FillMode("SOLID"), CullMode("BACK"), DepthWrite(true), StencilWrite(false)]
PS {
    PS_OUTPUT main(VS_OUTPUT input) 
    {
        cbuffer_t<SAMPLER_DATA> samplerData = make_cbuffer<SAMPLER_DATA>(gSamplerData);
        cbuffer_t<EFFECT_DATA> effectData = make_cbuffer<EFFECT_DATA>(gEffectData);

        PS_OUTPUT output;
        output.color = make_texture2D(effectData.Get().inputTexture).Get().Sample(make_sampler(samplerData.Get().linearSampler).Get(), input.uv);
        return output;
    }
}