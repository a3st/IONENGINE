// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

HEADER {
    Name = "Quad"
    Description = "Quad Shader"
    Domain = "Screen"
}

DATA {
    Texture2D inputTexture;
}

VS {
    #include "shared/common.hlsli"

    VS_OUTPUT main(VS_INPUT input) {
        VS_OUTPUT output;
        output.position = float4(output.uv * 2.0 + -1.0, 0.0, 1.0);
        output.uv = float2((input.id << 1) & 2, input.id & 2);
        return output;
    }
}

PS {
    #include "shared/common.hlsli"

    PS_OUTPUT main(VS_OUTPUT input) {
        Texture2D inputTexture = GetResource(material, inputTexture);
        SamplerState linearSampler = GetSampler();

        VS_OUTPUT output;
        output.color = inputTexture.Sample(linearSampler, input.uv);
        return output;
    }
}

OUTPUT {
    CullSide = "back";
    DepthWrite = false;
    StencilWrite = false;
}