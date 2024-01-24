import re
from shader import ShaderParser
import json

str = ''

with open("renderer/shaders/basic.hlsl", mode="r") as fp:
    str = fp.read()


parser = ShaderParser(str)
print(parser.stages)
print(parser.exports)