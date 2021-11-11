#pragma once

#include "Common.h"

struct D3D12ShaderCompilerInfo
{
	dxc::DxcDllSupport		DxcDllHelper;
	IDxcCompiler* compiler = nullptr;
	IDxcLibrary* library = nullptr;
};

namespace D3DShaders
{
	void Init_Shader_Compiler(D3D12ShaderCompilerInfo& shaderCompiler);
}