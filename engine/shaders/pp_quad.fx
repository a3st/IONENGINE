// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Quad";
    Description = "Output texture as Quad";
    Domain = "PostProcess";
}

DATA {
    uint inputTexture;
}

VS {
    VSOutput main(VSInput input) 
    {
        VSOutput output;
        output.uv = float2((input.id << 1) & 2, input.id & 2);
        output.position = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
        return output;
    }
}

[FillMode("SOLID"), CullMode("BACK"), DepthWrite(false), StencilWrite(false)]
PS {
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