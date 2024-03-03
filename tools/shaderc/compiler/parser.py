# Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

import re
import os
from compiler.base import SHADER_STAGES, DATA_TYPES, TYPE_SIZES


class ShaderParser:
    def __init__(self, path: str):
        self.include_pattern = re.compile(r"#include\s+\"(.+)\"")
        self.struct_pattern = re.compile(r"struct\s+(\w+)\s+{((?:\s+.+\s+){1,})};")
        self.struct_data_elements_pattern = re.compile(
            r"(bool|uint|float[2-4]x[2-4]|float[2-4]*)\s+(\w+);"
        )
        self.struct_ia_elements_pattern = re.compile(
            r"(bool|uint|float[2-4]x[2-4]|float[2-4]*)\s+(\w+)\s*:\s*(\w+)\s*;"
        )
        self.entry_func_pattern = re.compile(r"\w+\s+(\w+)\s*[(].+[)]")

        self.root_path = os.path.dirname(path)

        with open(path, mode="rt") as fp:
            data = fp.read()

        data = self.__merge_shader_(data)

        self.code = data
        self.stages = {}
        self.exports = {}

        matches = re.findall(self.entry_func_pattern, data)
        for match in matches:
            match match:
                case "vs_main":
                    self.stages["VERTEX_SHADER"] = {
                        "buffer": -1,
                        "entryPoint": match,
                        "inputs": [],
                        "inputsSizePerVertex": -1,
                    }
                case _:
                    self.stages[SHADER_STAGES[match]] = {
                        "buffer": -1,
                        "entryPoint": match,
                    }

        matches = re.findall(self.struct_pattern, data)
        for match in matches:
            match match[0]:
                case "ShaderData":
                    self.__extract_export_elements__(match[1])
                case "VSInput" | "VSOutput" | "PSOutput":
                    if match[0] != "VSInput":
                        continue
                    self.__parse_ia_buffer_elements__(match[1])

        for name in self.exports.keys():
            matches = re.findall(self.struct_pattern, data)
            for match in matches:
                if match[0] != name:
                    continue
                self.__parse_buffer_elements__(name, match[1])

    def __extract_export_elements__(self, elements: str):
        matches = re.findall(self.struct_data_elements_pattern, elements)
        offset = 0

        for match in matches:
            if match[0] == "uint":
                self.exports[match[1][0].upper() + match[1][1:]] = {
                    "binding": offset,
                    "type": "NON_UNIFORM",
                }
                offset += 1
            else:
                raise RuntimeError(
                    "ShaderData cannot contain data types other than uint"
                )

    def __parse_buffer_elements__(self, name: str, elements: str):
        matches = re.findall(self.struct_data_elements_pattern, elements)
        size = 0

        self.exports[name]["type"] = "UNIFORM"
        self.exports[name]["elements"] = []

        for match in matches:
            self.exports[name]["elements"].append(
                {
                    "name": match[1][0].upper() + match[1][1:],
                    "type": DATA_TYPES[match[0]],
                    "offset": size,
                }
            )
            size += TYPE_SIZES[match[0]]

        self.exports[name]["sizeInBytes"] = size

    def __parse_ia_buffer_elements__(self, elements: str):
        matches = re.findall(self.struct_ia_elements_pattern, elements)
        size = 0

        for match in matches:
            self.stages["VERTEX_SHADER"]["inputs"].append(
                {"type": DATA_TYPES[match[0]], "semantic": match[2]}
            )
            size += TYPE_SIZES[match[0]]

        self.stages["VERTEX_SHADER"]["inputsSizePerVertex"] = size

    def __merge_shader_(self, data: str) -> str:
        while True:
            result = re.search(self.include_pattern, data)
            if result is None:
                break

            with open(os.path.join(self.root_path, result.group(1)), mode="rt") as fp:
                include_data = fp.read()

            data = data[: result.start()] + include_data + data[result.end() :]
        return data
