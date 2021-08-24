# IONENGINE    [![Windows](https://github.com/a3st/ionengine_ops/actions/workflows/windows.yml/badge.svg?branch=main)](https://github.com/a3st/ionengine_ops/actions/workflows/windows.yml)
## Realtime Renderer Project
Lightweight renderer project for exploring and learning graphics api. This project is still development

## Features

- Module architecture
- Uses modern C++17 features, no legacy code, no raw pointers
- Renderer uses cross-api low level wrapper (D3D12/Vulkan)
- Minimum third-party dependinices
- Cross-platform (Linux/Windows/Android)

## Roadmap development

- [ ] framework/logger_system
- [x] framework/input_system
- [ ] framework/render_system
- [x] platform/win32/window
- [ ] platform/linux/window
- [ ] platform/android/window
- [x] lib/core
- [x] lib/math
- [x] renderer/api/d3d12
- [ ] renderer/api/vk
- [ ] renderer/frame_graph
