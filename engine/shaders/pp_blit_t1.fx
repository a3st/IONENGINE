// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Blit";
    Description = "Combining incoming textures into one (T1)";
    Domain = "PostProcess";
}

VS {
    #include "shared/ionengine.hlsli"

    VSOutput main(VSInput input) 
    {
        return QuadFromNullInput(input)
    }
}

[FillMode("SOLID"), CullMode("BACK"), DepthWrite(false), StencilWrite(false)]
PS {
    PSOutput main(VSOutput input) 
    {
        cbuffer_t<SAMPLER_DATA> samplerData = make_cbuffer<SAMPLER_DATA>(gSamplerData);
        cbuffer_t<PASS_DATA> passData = make_cbuffer<PASS_DATA>(gPassData);

        PSOutput output;
        output.color = make_texture2D(passData.Get().inputTexture1).Get().Sample(make_sampler(samplerData.Get().linearSampler).Get(), input.uv);
        return output;
    }
}