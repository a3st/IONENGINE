// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Base3D";
    Description = "Base3D";
    Domain = "Surface";
    Blend = "Opaque";
}

DATA {
    
}

VS {
    #include "shared/common.hlsli"

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
    #include "shared/common.hlsli"

    PS_OUTPUT main(VS_OUTPUT input) 
    {
        PS_OUTPUT output;
        output.color = float4(0.5f, 0.4f, 0.2f, 1.0f);
        return output;
    }
}