# <p align="center"> Paper for new Material / Shader System </p>

# What a pros
+ Conditional compilation shaders
+ The new type system allows you to see the same types as in the shader system, in HLSL
+ New macro functions allow you to make fewer mistakes
+ Dynamic type allows you to use both a cbuffer and a sampler at the same time (Useful when creating a flexible material)

# What a cons
- Requires a powerful shader caching system, otherwise you can get stuttering in the future
- This system is still more complex than the graph shader, but allows traditional coding rather than pasta design

## Data types
  Shader System - HLSL
  1. float_t - float
  2. float2_t - float2
  3. float3_t - float3
  4. float4_t - float4
  5. float4x4_t - float4x4
  6. bool_t - bool
  7. uint_t - uint

## Functions
  1. TEXTURE_SAMPLE(Texture, UV)

## Material shaders override
Each material allows you to override any pass of rendering. 
In the new system, the array with passes is hidden by default. By default, the engine uses standard shaders.

```mat
passes: [
    {
        name: "gbuffer",
        value: { path: "" }
    }
]
```