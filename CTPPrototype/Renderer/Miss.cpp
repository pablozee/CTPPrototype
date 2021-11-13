#include "Miss.h"
#include "D3DShaders.h"

namespace Miss
{
	void CreateMissProgram(D3D12Global& d3d, DXRGlobal& dxr, D3D12ShaderCompilerInfo& shaderCompiler)
	{
		// Load and compile the miss shader
		dxr.miss = RtProgram(D3D12ShaderInfo(L"shaders\\Miss.hlsl", L"", L"lib_6_3"));
		D3DShaders::CompileShader(shaderCompiler, dxr.miss);
	}
}