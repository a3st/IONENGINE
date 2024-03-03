# Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

import os
import io
import json
import struct
import tempfile
from compiler.base import BackendType
from compiler.parser import ShaderParser
from compiler.compiler import ShaderCompiler


class ChunkType:
    Json = 0
    Binary = 1


class ShaderFileFlags:
    DXIL = 0
    SPIRV = 1


class Serializer:
    Magic = b"SHADER.1"

    def __init__(self):
        pass

    def encode(self, shader_name: str, shader_path: str, to: BackendType) -> bytes:
        parser = ShaderParser(shader_path)
        compiler = ShaderCompiler(to)

        stream = io.BytesIO()
        offset = 0

        with tempfile.TemporaryFile(
            "wt", prefix="shaderc_", suffix=".tmp", delete=False
        ) as fp:
            fp.write(parser.code)

        for [stage, data] in parser.stages.items():
            try:
                output_path = os.path.join(
                    os.path.dirname(fp.name), "shaderc_wtfcode.tmp"
                )
                compiler.compile(fp.name, stage, data["entryPoint"], output_path)

                with open(output_path, mode="rb") as fp2:
                    shader_binary = fp2.read()

                os.unlink(fp2.name)

                stream.write(struct.pack("I", ChunkType.Binary))
                stream.write(struct.pack("I", len(shader_binary)))
                stream.write(shader_binary)

                data["buffer"] = offset

                offset += 1
            except RuntimeError as e:
                os.unlink(fp.name)
                raise RuntimeError(e)

        os.unlink(fp.name)

        metadata = {
            "shaderName": shader_name,
            "stages": parser.stages,
            "exports": parser.exports,
        }

        match to:
            case BackendType.DirectX12:
                flags = ShaderFileFlags.DXIL
            case BackendType.Vulkan:
                flags = ShaderFileFlags.SPIRV

        with io.BytesIO() as bp:
            bp.write(struct.pack("8s", Serializer.Magic))
            bp.write(struct.pack("I", 0))
            bp.write(struct.pack("I", flags))

            bp.write(struct.pack("I", ChunkType.Json))
            data = json.dumps(metadata).encode()
            bp.write(struct.pack("I", len(data)))
            bp.write(data)

            bp.write(stream.getvalue())

            size = bp.tell()
            bp.seek(len(Serializer.Magic), 0)
            bp.write(struct.pack("I", size))

            return bp.getvalue()
