#pragma once

#include "Common.h"

namespace D3DShaders
{
	void InitShaderCompiler(D3D12ShaderCompilerInfo& shaderCompiler);

	void CompileShader(D3D12ShaderCompilerInfo& compilerInfo, RtProgram& program);

	void CompileShader(D3D12ShaderCompilerInfo& compilerInfo, D3D12ShaderInfo& info, IDxcBlob** blob);
}