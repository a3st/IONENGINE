# Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

import os
import subprocess
from compiler.base import BackendType


class ShaderCompiler:
    TARGET_PROFILE_VERSION = "6_6"

    def __init__(self, backend_type: BackendType):
        match backend_type:
            case BackendType.DirectX12:
                self.compiler_path = os.path.join(os.path.dirname(__file__), "dxc")
            case BackendType.Vulkan:
                raise RuntimeError("Unsupported shader backend type")

    def compile(
        self, shader_path: str, target: str, entry_point: str, output_path: str
    ):
        match target:
            case "VERTEX_SHADER":
                target_options = "vs_" + ShaderCompiler.TARGET_PROFILE_VERSION
            case "PIXEL_SHADER":
                target_options = "ps_" + ShaderCompiler.TARGET_PROFILE_VERSION
            case _:
                raise RuntimeError("Unsupported shader target type")

        try:
            subprocess.check_output(
                [
                    os.path.join(self.compiler_path, "dxc.exe"),
                    shader_path,
                    "-T",
                    target_options,
                    "-E",
                    entry_point,
                    "-Fo",
                    output_path,
                ],
                stderr=subprocess.STDOUT,
            )
        except subprocess.CalledProcessError as e:
            print(e.output.decode())
            raise RuntimeError("Shader compilation failed")
