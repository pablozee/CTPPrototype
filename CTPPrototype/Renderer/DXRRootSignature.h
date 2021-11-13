#pragma once

#include "Common.h"

namespace DXRRootSignature
{
	ID3D12RootSignature* CreateRootSignature(D3D12Global& d3d, const D3D12_ROOT_SIGNATURE_DESC& desc);
}