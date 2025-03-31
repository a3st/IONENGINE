// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "UI (Texture)";
    Description = "Basic shader for UI";
    Domain = "UI";
}

DATA {
    uint inputTexture;
}

VS {
    VS_OUTPUT main(VS_INPUT input) 
    {
        cbuffer_t<TRANSFORM_DATA> transformData = make_cbuffer<TRANSFORM_DATA>(gTransformData);

        float4 worldPosition = float4(input.position, 0.0f, 1.0f);

        VS_OUTPUT output;
        output.position = mul(transformData.Get().modelViewProj, worldPosition);
        output.color = input.color;
        output.uv = input.uv;
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