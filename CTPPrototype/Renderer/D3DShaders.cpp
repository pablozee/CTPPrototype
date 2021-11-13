#include "D3DShaders.h"
#include "Utils.h"

#include <vector>
#include <string>

using namespace std;

namespace D3DShaders
{
	void InitShaderCompiler(D3D12ShaderCompilerInfo& shaderCompiler)
	{

		HRESULT hr = shaderCompiler.DxcDllHelper.Initialize();
		Utils::Validate(hr, L"Failed to initialize DxcDllSupport!");

		hr = shaderCompiler.DxcDllHelper.CreateInstance(CLSID_DxcCompiler, &shaderCompiler.compiler);
		Utils::Validate(hr, L"Failed to create DxcCompiler!");

		hr = shaderCompiler.DxcDllHelper.CreateInstance(CLSID_DxcLibrary, &shaderCompiler.library);
		Utils::Validate(hr, L"Failed to create DxcLibrary!");
	}

	void CompileShader(D3D12ShaderCompilerInfo& compilerInfo, D3D12ShaderInfo& info, IDxcBlob** blob)
	{
		HRESULT hr;
		UINT32 code(0);
		IDxcBlobEncoding* pShaderText(nullptr);

		// Load and encode the shader file
		hr = compilerInfo.library->CreateBlobFromFile(info.filename, &code, &pShaderText);
		Utils::Validate(hr, L"Error: failed to create blob from shader file!");

		// Create the compiler include handler
		IDxcIncludeHandler* dxcIncludeHandler;
		hr = compilerInfo.library->CreateIncludeHandler(&dxcIncludeHandler);
		Utils::Validate(hr, L"Error: failed to create include handler");

		// Compile the shader
		IDxcOperationResult* result;
		hr = compilerInfo.compiler->Compile(
			pShaderText,
			info.filename,
			info.entryPoint,
			info.targetProfile,
			info.arguments,
			info.argCount,
			info.defines,
			info.defineCount,
			dxcIncludeHandler,
			&result);

		Utils::Validate(hr, L"Error: failed to compile shader!");

		// Verify the result
		result->GetStatus(&hr);
		if (FAILED(hr))
		{
			IDxcBlobEncoding* error;
			hr = result->GetErrorBuffer(&error);
			Utils::Validate(hr, L"Error: failed to get shader compiler error buffer!");

			// Convert error blob to a string
			vector<char> infoLog(error->GetBufferSize() + 1);
			memcpy(infoLog.data(), error->GetBufferPointer(), error->GetBufferSize());
			infoLog[error->GetBufferSize()] = 0;

			string errorMsg = "Shader Compiler Error:\n";
			errorMsg.append(infoLog.data());

			MessageBoxA(nullptr, errorMsg.c_str(), "Error!", MB_OK);
			return;
		}

		hr = result->GetResult(blob);
		Utils::Validate(hr, L"Error: failed to get shader blob result!");
	}

	/**
	* Compile an HLSL ray tracing shader using dxcompiler.
	*/
	void CompileShader(D3D12ShaderCompilerInfo& compilerInfo, RtProgram& program)
	{
		CompileShader(compilerInfo, program.info, &program.blob);
		program.SetBytecode();
	}
}