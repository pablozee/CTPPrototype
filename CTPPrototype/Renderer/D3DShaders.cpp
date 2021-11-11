#include "D3DShaders.h"
#include "Utils.h"

namespace D3DShaders
{
	void Init_Shader_Compiler(D3D12ShaderCompilerInfo& shaderCompiler)
	{

		HRESULT hr = shaderCompiler.DxcDllHelper.Initialize();
		Utils::Validate(hr, L"Failed to initialize DxcDllSupport!");

		hr = shaderCompiler.DxcDllHelper.CreateInstance(CLSID_DxcCompiler, &shaderCompiler.compiler);
		Utils::Validate(hr, L"Failed to create DxcCompiler!");

		hr = shaderCompiler.DxcDllHelper.CreateInstance(CLSID_DxcLibrary, &shaderCompiler.library);
		Utils::Validate(hr, L"Failed to create DxcLibrary!");
	}
}