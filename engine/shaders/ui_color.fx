// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "UI (Color)";
    Description = "Basic shader for UI";
    Domain = "UI";
}

VS {
    VS_OUTPUT main(VS_INPUT input) 
    {
        cbuffer_t<TRANSFORM_DATA> transformData = make_cbuffer<TRANSFORM_DATA>(gTransformData);

        float4 worldPosition = float4(input.position, 0.0f, 1.0f);

        VS_OUTPUT output;
        output.position = mul(transformData.Get().modelViewProj, worldPosition);
        output.uv = input.uv;
        output.color = input.color;
        return output;
    }
}

[FillMode("SOLID"), CullMode("FRONT"), DepthWrite(false), StencilWrite(false)]
PS {
    PS_OUTPUT main(VS_OUTPUT input) 
    {
        PS_OUTPUT output;
        output.color = input.color;
        return output;
    }
}