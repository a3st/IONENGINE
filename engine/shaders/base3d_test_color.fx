// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Base3D";
    Description = "Base3D Test Color";
    Domain = "Surface";
    Blend = "Opaque";
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
        PS_OUTPUT output;
        output.color = float4(1.0f, 1.0f, 0.5f, 1.0f);
        return output;
    }
}