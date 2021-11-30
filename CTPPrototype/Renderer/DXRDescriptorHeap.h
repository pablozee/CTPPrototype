#pragma once

#include "Common.h"

namespace DXRDescriptorHeap
{
	void CreateDescriptorHeaps(D3D12Global& d3d, DXRGlobal& dxr, D3D12Resources& resources, const std::vector<Model>& modelsVec, const std::vector<Material>& materials);
}