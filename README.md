# <p align="center"> IONENGINE </p>
## <p align="center"> Realtime Renderer Project </p>
Lightweight renderer project for exploring and learning Graphics API

## Features

- Module architecture
- C++latest
- Graphics API backend
- Deffered/forward shading PBR
- Flexible material system
- Asset system with thread pool
- Async upload manager for GPU resources
- Minimum thirdparty dependinices

## Priority tasks

Renderer
  - [x] D3D12 backend
  - [ ] Pipeline disk cache
  - [ ] Upload manager /w Transition barrier engine
  - [x] Frame graph (only render pass support)
  - [x] RmlUI renderer integration
  - [x] Physically-based rendering
  - [x] Deffered shading
  - [x] Forward shading
  - [x] FXAA
  - [ ] TAA
  - [x] Material system
  - [ ] Spot light
  - [ ] Directional light
  - [x] Point light
  - [ ] Shadows

Library / platform
  - [x] Math library
  - [x] Cross-platform windowing / input (only Windows support)
  - [x] Thread pool
  - [x] Logger
  - [x] Channel / event dispatcher
  - [x] Expected / error handling

Input
  - [x] Base input system
  - [ ] Gamepad support

Asset
  - [x] Asset manager
  - [x] Asset loader
  - [x] Shader
  - [x] Texture
  - [x] Mesh
  - [x] Material
  - [ ] Subscene
  - [ ] Widget (User interface)
