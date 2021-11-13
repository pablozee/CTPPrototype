#pragma once

#include "Common.h"

namespace ConstantBuffer
{
	void CreateConstantBuffer(D3D12Global& d3d, ID3D12Resource** buffer, UINT64 size);
}