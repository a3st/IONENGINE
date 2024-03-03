# Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.


class BackendType:
    DirectX12 = 0
    Vulkan = 1


DATA_TYPES = {
    "float4x4": "FLOAT4x4",
    "float3x3": "FLOAT3x3",
    "float2x2": "FLOAT2x2",
    "float4": "FLOAT4",
    "float3": "FLOAT3",
    "float2": "FLOAT2",
    "float": "FLOAT",
    "uint": "UINT",
    "bool": "BOOL",
}

SHADER_STAGES = {"vs_main": "VERTEX_SHADER", "ps_main": "PIXEL_SHADER"}

TYPE_SIZES = {
    "float4x4": 64,
    "float3x3": 36,
    "float2x2": 16,
    "float4": 16,
    "float3": 12,
    "float2": 8,
    "float": 4,
    "uint": 4,
    "bool": 2,
}