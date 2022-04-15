
#include <precompiled.h>
#include <shader/compiler.h>

#include <lib/exception.h>

#define NOMINMAX
#define UNICODE
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <dxcapi.h>

#ifndef THROW_IF_FAILED
#define THROW_IF_FAILED(Result) if(FAILED(Result)) throw ionengine::lib::Exception(ionengine::renderer::backend::d3d12::hresult_to_string(Result));
#endif // THROW_IF_FAILED

using Microsoft::WRL::ComPtr;

using namespace ionengine::shader;

struct ShaderCompiler::Impl {

    ComPtr<IDxcCompiler3> compiler;

    void initialize();

    bool compile(std::filesystem::path const& shader_path);
};

void ShaderCompiler::impl_deleter::operator()(Impl* ptr) const {

    delete ptr;
}

void ShaderCompiler::Impl::initialize() {

    DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), reinterpret_cast<void**>(compiler.GetAddressOf()));
}

bool ShaderCompiler::Impl::compile(std::filesystem::path const& shader_path) {

    
    return true;
}

ShaderCompiler::ShaderCompiler() :
    _impl(std::unique_ptr<Impl, impl_deleter>(new Impl())) {

    _impl->initialize();
}

bool ShaderCompiler::compile(std::filesystem::path const& shader_path) {

    
    return true;
}