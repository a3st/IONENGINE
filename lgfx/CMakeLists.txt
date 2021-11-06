# Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

cmake_minimum_required(VERSION 3.0.0)
project(lgfx VERSION 1.0.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

# D3D12 Build
include(${PROJECT_SOURCE_DIR}/d3d12/CMakeLists.txt)

# Vulkan Build

# OpenGL Build
