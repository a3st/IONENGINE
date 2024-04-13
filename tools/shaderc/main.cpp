
#include "precompiled.h"
#include <d3d12.h>
#include <d3d12shader.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "dxgi.lib")

auto read_file(std::filesystem::path const& file_path) -> std::vector<uint8_t>
{
    std::vector<uint8_t> out;
    {
        std::basic_ifstream<uint8_t> stream(file_path, std::ios::binary);
        stream.seekg(0, std::ios::end);
        uint64_t const offset = stream.tellg();
        out.resize(offset);
        stream.seekg(0, std::ios::beg);
        stream.read(out.data(), out.size());
    }
    return out;
}

auto main(int32_t argc, char** argv) -> int32_t
{
    winrt::com_ptr<IDxcCompiler3> compiler;
    ::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), compiler.put_void());

    auto source = read_file("shader.hlsl");

    DxcBuffer buffer;
    buffer.Ptr = source.data();
    buffer.Size = source.size();
    buffer.Encoding = DXC_CP_ACP;

    std::vector<LPCWSTR> arguments;
    arguments.emplace_back(L"-T vs_6_0");
    arguments.emplace_back(L"-E vs_main");
    arguments.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL3);

    winrt::com_ptr<IDxcResult> result;
    compiler->Compile(&buffer, arguments.data(), static_cast<uint32_t>(arguments.size()), nullptr, __uuidof(IDxcResult),
                      result.put_void());

    winrt::com_ptr<IDxcBlobUtf8> errors;
    result->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), errors.put_void(), nullptr);

    if (errors && errors->GetStringLength() > 0)
    {
        std::cerr << std::string_view(reinterpret_cast<char*>(errors->GetBufferPointer()), errors->GetBufferSize())
                  << std::endl;
        return EXIT_FAILURE;
    }

    winrt::com_ptr<IDxcBlob> shader;
    result->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), shader.put_void(), nullptr);

    D3D12_SHADER_BYTECODE d3d12_shader_bytecode;
    d3d12_shader_bytecode.pShaderBytecode = shader->GetBufferPointer();
    d3d12_shader_bytecode.BytecodeLength = shader->GetBufferSize();

    winrt::com_ptr<IDxcUtils> utils;
    ::DxcCreateInstance(CLSID_DxcUtils, __uuidof(IDxcUtils), utils.put_void());

    winrt::com_ptr<IDxcBlob> reflect;
    result->GetOutput(DXC_OUT_REFLECTION, __uuidof(IDxcBlob), reflect.put_void(), nullptr);

    DxcBuffer reflect_buffer;
    reflect_buffer.Ptr = reflect->GetBufferPointer();
    reflect_buffer.Size = reflect->GetBufferSize();
    reflect_buffer.Encoding = DXC_CP_ACP;

    winrt::com_ptr<ID3D12ShaderReflection> shader_reflection;
    utils->CreateReflection(
        &reflect_buffer, 
        __uuidof(ID3D12ShaderReflection), 
        shader_reflection.put_void()
    );

    D3D12_SHADER_DESC d3d12_shader_desc;
    shader_reflection->GetDesc(&d3d12_shader_desc);

    for (uint32_t const index : std::views::iota(0u, d3d12_shader_desc.ConstantBuffers))
    {
        D3D12_SHADER_BUFFER_DESC d3d12_shader_buffer_desc;
        shader_reflection->GetConstantBufferByIndex(0)->GetDesc(&d3d12_shader_buffer_desc);

        std::cout << std::format("BufferName: {}, BufferSize: {}", 
                                 d3d12_shader_buffer_desc.Name,
                                 d3d12_shader_buffer_desc.Size)
                  << std::endl;
    }
    return EXIT_SUCCESS;
}