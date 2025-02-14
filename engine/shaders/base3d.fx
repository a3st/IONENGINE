// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Base3D";
    Description = "Base3D";
    Domain = "Surface";
    Blend = "Opaque";
}

DATA {
    float4 color;
}

VS {
    #include "shared/common.hlsli"

    VS_OUTPUT main(VS_INPUT input) {
        VS_OUTPUT output;
        
        return output;
    }
}