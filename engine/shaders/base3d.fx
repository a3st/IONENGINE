// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Base3D";
    Description = "Base3D";
    Domain = "Surface";
    Blend = "Opaque";
}

DATA {
    texture2D_t basicTex;
}

VS {
    VS_OUTPUT main(VS_INPUT input) 
    {
        cbuffer_t<TRANSFORM_DATA> transformBuffer = gShaderData.transformBuffer;

        float4 worldPosition = float4(input.position, 1.0f);

        VS_OUTPUT output;
        output.position = mul(transformBuffer.Get().modelViewProj, worldPosition);
        output.normal = input.normal;
        output.uv = input.uv;
        return output;
    }
}

[FillMode("SOLID"), CullSide("BACK"), DepthWrite(true), StencilWrite(false)]
PS {
    PS_OUTPUT main(VS_OUTPUT input)
    {
        cbuffer_t<SAMPLER_DATA> samplerBuffer = gShaderData.samplerBuffer;
        cbuffer_t<MATERIAL_DATA> materialBuffer = gShaderData.materialBuffer;

        PS_OUTPUT output;
        output.color = materialBuffer.Get().basicTex.Get().Sample(samplerBuffer.Get().linearSampler.Get(), input.uv).rgba;
        return output;
    }
}