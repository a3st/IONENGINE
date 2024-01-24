import re

data_types = {
    'float4x4':     "FLOAT4x4",
    'float3x3':     "FLOAT3x3",
    'float2x2':     "FLOAT2x2",
    'float4':       "FLOAT4",
    'float3':       "FLOAT3",
    'float2':       "FLOAT2",
    "float":        "FLOAT",
    "uint":         "UINT",
    "bool":         "BOOL"
}

shader_stages = {
    'vs_main': 'VERTEX_SHADER',
    'ps_main': 'PIXEL_SHADER'
}

data_type_sizes = {
    'float4x4':     64,
    'float3x3':     36,
    'float2x2':     16,
    'float4':       16,
    'float3':       12,
    'float2':       8,
    "float":        4,
    "uint":         4,
    "bool":         2
}

class ShaderParser:
    
    def __init__(self, data: str):
        self.stages = {}
        self.exports = {}

        self.struct_pattern = re.compile(r'struct\s+(\w+)\s+{((?:\s+.+\s+){1,})};')
        self.struct_data_elements_pattern = re.compile(r'(bool|uint|float[2-4]x[2-4]|float[2-4]*)\s+(\w+);')
        self.struct_ia_elements_pattern = re.compile(r'(bool|uint|float[2-4]x[2-4]|float[2-4]*)\s+(\w+)\s*:\s*(\w+)\s*;')
        self.entry_func_pattern = re.compile(r'\w+\s+(\w+)\s*[(].+[)]')

        matches = re.findall(self.entry_func_pattern, data)

        for match in matches:
            match match:
                case 'vs_main':
                    self.stages['VERTEX_SHADER'] = {
                        'entryPoint': match,
                        'inputs': []
                    }
                case _:
                    self.stages[shader_stages[match]] = {
                        'entryPoint': match
                    }

        matches = re.findall(self.struct_pattern, data)

        for match in matches:
            match match[0]:
                case 'ShaderResources':
                    self.__extract_export_elements__(match[1])
                case 'VSInput' | 'VSOutput' | 'PSOutput':
                    if match[0] != 'VSInput':
                        continue
                    self.__parse_ia_buffer_elements__(match[1])
                case _:
                    self.__parse_buffer_elements__(match[0], match[1])


    def __extract_export_elements__(self, elements: str):
        matches = re.findall(self.struct_data_elements_pattern, elements)
        offset = 0

        for match in matches:
            if match[0] == 'uint':
                self.exports[match[1][0].upper() + match[1][1:]] = {
                    'binding': offset,
                    'type': 'TEXTURE_UAV'
                }
                offset += 1
            else:
                raise TypeError("ShaderResources cannot contain data types other than uint")
            
    def __parse_buffer_elements__(self, name: str, elements: str):
        matches = re.findall(self.struct_data_elements_pattern, elements)
        size = 0

        self.exports[name]['type'] = 'BUFFER'
        self.exports[name]['elements'] = []

        for match in matches:
            self.exports[name]['elements'].append({
                'name': match[1],
                'type': data_types[match[0]]
            })
            size += data_type_sizes[match[0]]

        self.exports[name]['sizeInBytes'] = size


    def __parse_ia_buffer_elements__(self, elements: str):
        matches = re.findall(self.struct_ia_elements_pattern, elements)
        size = 0

        for match in matches:
            self.stages['VERTEX_SHADER']['inputs'].append({
                'type': data_types[match[0]],
                'semantic': match[2]
            })
            size += data_type_sizes[match[0]]
        
        self.stages['VERTEX_SHADER']['inputsSizePerVertex'] = size
