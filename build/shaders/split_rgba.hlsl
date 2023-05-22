// shader-function: Split_RGBA
#pragma once

struct split_rgba_in_t {
      // serialize-field
      // name: "Source"
      // description: "Split into components"
      float4 source;
};

struct split_rgba_out_t {
      // serialize-field
      // name: "RGB"
      // description: "RGB"
      float3 rgb;
      // serialize-field
      // name: "Alpha"
      // description: "Alpha"
      float a;
};

split_rgba_out_t split_rgba_main(split_rgba_in_t input) {
      split_rgba_out_t output;
      output.rgb = input.source.rgb;
      output.a = input.source.a;
      return output;
}