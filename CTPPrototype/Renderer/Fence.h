#pragma once

#include "DirectXStructures.h"

namespace Fence
{
	void CreateFence(D3D12Global& d3d);

	void WaitForGPU(D3D12Global& d3d);

	void MoveToNextFrame(D3D12Global& d3d);
}