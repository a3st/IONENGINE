# Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

import sys
import argparse
import os
from compiler.base import BackendType
from compiler.serializer import Serializer

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        "IONENGINE Shader Compiler", description="Tool for shader compilation"
    )
    parser.add_argument("-n", type=str, help="Name for new shader", required=True)
    parser.add_argument("-i", type=str, help="Path to .hlsl shader code", required=True)
    parser.add_argument(
        "-o", type=str, help="Output path to shader binary", required=False, default=""
    )
    parser.add_argument(
        "-t", type=str, help="Target backend compilation", required=True
    )
    args = parser.parse_args()

    match args.t:
        case "DX12":
            backend_type = BackendType.DirectX12
        case "VK":
            backend_type = BackendType.Vulkan
        case _:
            raise RuntimeError("Unsupported shader backend type")

    try:
        serializer = Serializer()
        output = serializer.encode(args.n, args.i, backend_type)

        with open(os.path.join(args.o, args.n.lower() + ".bin"), mode="wb") as fp:
            fp.write(output)

    except RuntimeError as e:
        print(e, file=sys.stderr)
