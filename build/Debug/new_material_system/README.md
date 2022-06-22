# <p align="center"> White-paper for new Material / Shader System </p>

# What a pros
+ Conditional compilation shaders
+ The new type system allows you to see the same types as in the shader system, in HLSL
+ New macro functions allow you to make fewer mistakes
+ Dynamic type allows you to use both a cbuffer and a sampler at the same time (Useful when creating a flexible material)

# What a cons
- Requires a powerful shader caching system, otherwise you can get stuttering in the future
- This system is still more complex than the shader graph, but allows traditional coding rather than pasta design

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
        path: "content/shaders/gbuffer_custom.shader"
    }
]
```

## Shader Variant it's new Shader class
Shader variants is one of ocondition. Every shader by default contains the following conditions:
  1. ```HAS_STATIC = 1 << 0```
  2. ```HAS_SKIN = 1 << 1```
  3. ```HAS_RESERVED = 1 << 2```

With each condition, the shader becomes difficult to compile. Condition limit by ```10``` per one shader.
```c++
inline constexpr CONDITION_HAS_STATIC = 1 << 0;
inline constexpr CONDITION_HAS_SKIN = 1 << 1;
```
In order to access the shader variant you need call ```shader->get().variants.at(CONDITION)```

Each material generates its own conditionality depending on the current parameters. By default is ```HAS_STATIC``` if material has not dynamic parameters.

To get the current condition you need call ```material->get().condition```

As a result, you can find out the final conditionality using the following code:
```c++
AssetPtr<Material> material = asset_manager.get_material("content/base.mat");
AssetPtr<Shader> shader = material->get().passes.at("gbuffer");
ShaderVariant& variant = shader->get().variants.at(material->get().condition);
ShaderVariant& variant = shader->get().variants.at(HAS_SKIN | material->get().condition);
```
Thanks to the generated flags and the std::unordered_map, you can make a quick selection.

## Material run-time change parameters
The big problem is changing parameters at run-time. If you will change dynamic parameters, material should change condition variable.

Example: roughness is dynamic parameter, shader set contains uniforms only for float and sampler2D 
```c++
// dynamic cast usage
material->get().parameters.at("roughness").as_dynamic<MaterialParameterType::Float>() = 1.0f;
material->get().parameters.at("roughness").as_dynamic<MaterialParameterType::Sampler2D>() = asset_manager.get_texture("content/base.dds");
material->get().parameters.at("roughness").as_dynamic<MaterialParameterType::Float2>() = // invalid cast, throw exception

// static cast usage
material->get().parameters.at("water_scale").as_static<MaterialParameterType::Float>() = 2.0f;

// static cast usage as dynamic
material->get().parameters.at("water_scale").as_dynamic<MaterialParameterType::Float>() = // invalid cast, throw exception
```
