#pragma once

#include "Buffer.h"

namespace VertexBuffer
{
	void CreateVertexBuffer(D3D12Global& d3d, D3D12Resources& resources, std::vector<Model>& modelsVec);
}