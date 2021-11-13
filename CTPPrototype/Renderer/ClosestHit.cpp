#include "ClosestHit.h"
#include "D3DShaders.h"

namespace ClosestHit
{
	void CreateClosestHitProgram(D3D12Global& d3d, DXRGlobal& dxr, D3D12ShaderCompilerInfo& shaderCompiler)
	{
		// Load and compile the Closest Hit shader
		dxr.hit = HitProgram(L"Hit");
		dxr.hit.chs = RtProgram(D3D12ShaderInfo(L"shaders\\ClosestHit.hlsl", L"", L"lib_6_3"));
		D3DShaders::CompileShader(shaderCompiler, dxr.hit.chs);
	}
}