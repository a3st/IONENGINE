// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Quad";
    Description = "Output texture as quad";
    Domain = "PostProcess";
}

DATA {
    uint inputTexture;
}

VS {
    #include "shared/ionengine.hlsli"

    VSOutput main(VSInput input) 
    {
        return QuadFromNullInput(input);
    }
}

[FillMode("SOLID"), CullMode("BACK"), DepthWrite(false), StencilWrite(false)]
PS {
    #include "shared/ionengine.hlsli"

    PSOutput main(VSOutput input)
    {
        cbuffer_t<SamplerData> samplerData = CreateResource< cbuffer_t<SamplerData> >(gSamplerData);
        cbuffer_t<EffectData> effectData = CreateResource< cbuffer_t<EffectData> >(gEffectData);

        PSOutput output;
        output.color = CreateResource<texture2D_t>(effectData.Get().inputTexture).Get().Sample(
            CreateResource<sampler_t>(samplerData.Get().linearSampler).Get(), input.uv);
        return output;
    }
}