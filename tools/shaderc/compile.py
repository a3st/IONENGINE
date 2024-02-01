import os
import io
import json
import sys
import argparse
import struct
from shader import ShaderParser, ShaderCompiler, BackendType

SHADER_FILE_MAGIC = b'SHADER.1'

class ChunkType:
    JSON = 0
    BIN = 1


class ShaderFileFlags:
    DXIL_BINARY = 0
    SPIRV_BINARY = 1


class ShaderFile:
    def __init__(self):
        self.cache_path = os.path.join(os.path.dirname(__file__), '__cache__')
        os.makedirs(self.cache_path, exist_ok=True)

    
    def encode(self, shader_name: str, shader_path: str, to: BackendType) -> bytes:
        with open(shader_path, mode='rt') as fp:
            shader_code = fp.read()

        parser = ShaderParser(shader_code)
        compiler = ShaderCompiler(to)

        for (stage, data) in parser.stages.items():
            compiler.compile(shader_path, stage, data['entryPoint'], self.cache_path)

        metadata = {
            'shaderName': shader_name,
            'stages': parser.stages,
            'exports': parser.exports
        }

        stages = sorted(parser.stages.items(), key=lambda x: x[1]['buffer'])

        match backend_type:
            case BackendType.DirectX12:
                flags = ShaderFileFlags.DXIL_BINARY
            case BackendType.Vulkan:
                flags = ShaderFileFlags.SPIRV_BINARY

        with io.BytesIO() as bp:
            bp.write(struct.pack('8s', SHADER_FILE_MAGIC))
            bp.write(struct.pack('I', 0))
            bp.write(struct.pack('I', flags))

            bp.write(struct.pack('I', ChunkType.JSON))
            data = json.dumps(metadata).encode()
            bp.write(struct.pack('I', len(data)))
            bp.write(data)

            for (stage, data) in stages:
                with open(os.path.join(self.cache_path, data['entryPoint'] + ".bin"), mode='rb') as fp:
                    shader_bytes = fp.read()

                bp.write(struct.pack('I', ChunkType.BIN))
                bp.write(struct.pack('I', len(shader_bytes)))
                bp.write(shader_bytes)

            size = bp.tell()
            bp.seek(len(SHADER_FILE_MAGIC), 0)
            bp.write(struct.pack('I', size))

            output = bp.getvalue()

        return output


if __name__ == "__main__":
    parser = argparse.ArgumentParser("IONENGINE Shader Compiler", description="Tool for shader compilation")
    parser.add_argument('-n', type=str, help="Name for new shader", required=True)
    parser.add_argument('-i', type=str, help="Path to .hlsl shader code", required=True)
    parser.add_argument('-l', type=str, help="Path to .hlsli shader files", required=False)
    parser.add_argument('-o', type=str, help="Output path to shader binary", required=False, default="")
    parser.add_argument('-t', type=str, help="Target backend compilation", required=True)
    args = parser.parse_args()

    match args.t:
        case 'DX12':
            backend_type = BackendType.DirectX12
        case 'VK':
            backend_type = BackendType.Vulkan
        case _:
            raise RuntimeError("Unsupported shader backend type")

    try:
        shader_file = ShaderFile()
        output = shader_file.encode(args.n, args.i, backend_type)

        with open(os.path.join(args.o, args.n.lower() + ".bin"), mode='wb') as fp:
            fp.write(output)

    except RuntimeError as e:
        print(e, file=sys.stderr)
