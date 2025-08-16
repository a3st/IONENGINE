// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "UI";
    Description = "Base shader for User Interface";
    Domain = "UI";
    Features = [ "TextureOnly" ];
}

DATA {
    uint inputTexture;
}

VS {
    VSOutput main(VSInput input) 
    {
        cbuffer_t<TransformData> transformData = CreateResource< cbuffer_t<TransformData> >(gTransformData);

        VSOutput output;
        float4 worldPosition = float4(input.position, 0.0f, 1.0f);
        output.position = mul(transformData.Get().modelViewProj, worldPosition);
        output.uv = input.uv;
        output.color = input.color;
        return output;
    }
}

[FillMode("SOLID"), CullMode("FRONT"), DepthWrite(false), StencilWrite(false)]
PS {
    PSOutput main(VSOutput input) 
    {
        PSOutput output;
        #ifdef USE_TEXTURE_ONLY
            cbuffer_t<SamplerData> samplerData = CreateResource< cbuffer_t<SamplerData> >(gSamplerData);
            cbuffer_t<EffectData> effectData = CreateResource< cbuffer_t<EffectData> >(gEffectData);

            output.color = input.color * CreateResource<texture2D_t>(effectData.Get().inputTexture).Get().Sample(
                CreateResource<sampler_t>(samplerData.Get().linearSampler).Get(), input.uv);
        #else
            output.color = input.color;
        #endif
        return output;
    }
}