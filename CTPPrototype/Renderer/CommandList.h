#pragma once

#include "DirectXStructures.h"

namespace CommandList
{
	void CreateCommandList(D3D12Global& d3d);
	
	void ResetCommandList(D3D12Global& d3d);

	void BuildCommandList(D3D12Global& d3d, DXRGlobal& dxr, D3D12Resources& resources);
	
	void SubmitCommandList(D3D12Global& d3d);
}