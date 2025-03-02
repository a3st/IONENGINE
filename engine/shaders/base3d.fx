// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Base3D";
    Description = "Base3D";
    Domain = "Surface";
    Blend = "Opaque";
}

DATA {
    texture2D_t basicTexture;
}

VS {
    VS_OUTPUT main(VS_INPUT input) 
    {
        cbuffer_t<TRANSFORM_DATA> transformData = gShaderData.transformData;

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
        cbuffer_t<SAMPLER_DATA> samplerData = gShaderData.samplerData;
        cbuffer_t<EFFECT_DATA> effectData = gShaderData.effectData;

        PS_OUTPUT output;
        output.color = effectData.Get().basicTexture.Get().Sample(samplerData.Get().linearSampler.Get(), input.uv).rgba;
        return output;
    }
}