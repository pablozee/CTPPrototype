#pragma once
#include <Windows.h>
#include "DirectXStructures.h"
#include "Structures.h"
#include "D3DShaders.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Initialize(HWND hwnd);
	void Update();
	void Draw();

	HWND GetHwnd() const { return m_Hwnd; }

private:
	void Shutdown();

	HWND m_Hwnd;

	D3D12Global d3d = {};
	D3D12Resources resources = {};
	DXRGlobal dxr = {};
	D3D12ShaderCompilerInfo shaderCompiler;

	Model model;
	Material material;
};