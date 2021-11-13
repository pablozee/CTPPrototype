#pragma once

#include "Structures.h"

namespace Buffer
{
	void CreateBuffer(D3D12Global& d3d, D3D12BufferCreateInfo& info, ID3D12Resource** ppResource);
}