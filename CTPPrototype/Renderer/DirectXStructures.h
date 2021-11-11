#pragma once

#include "Common.h"

using Microsoft::WRL::ComPtr;

struct D3D12Global
{
	ComPtr<ID3D12Device>	device;
	ComPtr<IDXGIFactory4>	factory;
	ComPtr<ID3D12Fence>		fence;
	
	int						width;
	int						height;
	bool					vsync;
};
