#pragma once

#include "DirectXStructures.h"

namespace CommandList
{
	void CreateCommandList(D3D12Global& d3d);
	
	void ResetCommandList(D3D12Global& d3d);
}