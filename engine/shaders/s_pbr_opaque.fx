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
    #include "shared/ionengine.hlsli"

    VsOutput main(VSInput input) 
    {
        #ifdef USE_SKINNING
        return SkeletalSurfaceFromInput(input);
        #else
        return StaticSurfaceFromInput(input);
        #endif
    }
}

[FillMode("SOLID"), CullMode("BACK"), DepthWrite(false), StencilWrite(false)]
PS {
    PSOutput main(VSOutput input)
    {
        PSOutput output;
        return output;
    }
}