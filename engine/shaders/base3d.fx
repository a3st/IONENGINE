// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Base3D";
    Description = "Base3D";
    Domain = "Surface";
    Blend = "Opaque";
}

DATA {
    uint basicTexture;
}

VS {
    VS_OUTPUT main(VS_INPUT input) 
    {
        cbuffer_t<TRANSFORM_DATA> transformData = make_cbuffer<TRANSFORM_DATA>(gTransformData);

        float4 worldPosition = float4(input.position, 1.0f);

        VS_OUTPUT output;
        output.position = mul(transformData.Get().modelViewProj, worldPosition);
        output.normal = input.normal;
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
        output.color = make_texture2D(effectData.Get().basicTexture).Get().Sample(make_sampler(samplerData.Get().linearSampler).Get(), input.uv);
        return output;
    }
}