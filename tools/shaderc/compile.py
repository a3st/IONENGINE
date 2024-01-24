import regex
from shader import ShaderParser

str = ''

with open("tools/shaderc/basic.hlsl", mode="r") as fp:
    str = fp.read() 

shader = ShaderParser()

regex.Regex('')