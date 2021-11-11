#pragma once

#include "Common.h"

struct D3D12Global
{
	IDXGIFactory4*	factory;
	IDXGIAdapter1*	adapter;
	ID3D12Device5*	device;
	ID3D12Fence*	fence;
	ID3D12CommandQueue* cmdQueue = nullptr;
	ID3D12CommandAllocator* cmdAlloc[2] = { nullptr, nullptr };
	
	int						width;
	int						height;
	bool					vsync;
};
