#pragma once

#include "Buffer.h"

namespace IndexBuffer
{
	void CreateIndexBuffer(D3D12Global& d3d, D3D12Resources& resources, std::vector<Model>& modelsVec);
}