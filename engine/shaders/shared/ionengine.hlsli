// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

VSOutput QuadFromNullInput(VSInput input) {
    VSOutput output;
    output.uv = float2((input.id << 1) & 2, input.id & 2);
    output.position = float4(output.uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}