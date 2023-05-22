// shader-function: PBR
#pragma once

struct pbr_in_t {
    // serialize-field
    // name: "Albedo"
    // description: "Albedo input color"
    float4 albedo;
    // serialize-field
    // name: "Normal"
    // description: "Normal input color"
    float3 normal;
    // serialize-field
    // name: "Roughness"
    // description: "Roughness input color"
    float roughness;
};

struct pbr_out_t {
    // serialize-field
    // name: "Color"
    // description: "Output color" 
    float4 color;
};

pbr_out_t pbr_main(pbr_in_t input) {
    pbr_out_t output;
    output.color = input.albedo;
    return output;
}