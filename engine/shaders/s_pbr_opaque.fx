// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "PBR";
    Description = "PBR Standard";
    Domain = "Surface";
    Blend = "Opaque";
    Features = [ "Skinning" ];
}

DATA {
    
}

VS {
    VSOutput main(VSInput input) 
    {
        #ifdef USE_SKINNING
        VSOutput output;
        output.position = float4(1.0f, 1.0f, 1.0f, 1.0f);
        output.normal = float3(1.0f, 1.0f, 1.0f);
        output.uv = input.uv;
        return output;
        #else
        VSOutput output;
        output.position = float4(1.0f, 1.0f, 1.0f, 1.0f);
        output.normal = float3(1.0f, 1.0f, 1.0f);
        output.uv = input.uv;
        return output;
        #endif
    }
}

[FillMode("SOLID"), CullMode("BACK"), DepthWrite(false), StencilWrite(false)]
PS {
    PSOutput main(VSOutput input)
    {
        PSOutput output;
        output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        return output;
    }
}