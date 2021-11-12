#pragma once

#include "Common.h"

struct D3D12Global
{
	IDXGIFactory4*						factory;
	IDXGIAdapter1*						adapter;
	ID3D12Device5*						device;
	ID3D12GraphicsCommandList4*			cmdList = nullptr;
	ID3D12CommandQueue*					cmdQueue = nullptr;
	ID3D12CommandAllocator*				cmdAlloc[2] = { nullptr, nullptr };

	IDXGISwapChain3*					swapChain = nullptr;
	ID3D12Resource*						backBuffer[2] = { nullptr, nullptr };
	
	ID3D12Fence*						fence;
	UINT64								fenceValues[2] = { 0,0 };
	HANDLE								fenceEvent;
	UINT								frameIndex = 0;
	
	int									width;
	int									height;
	bool								vsync;
};

struct D3D12Resources
{
	ID3D12DescriptorHeap*				rtvHeap = nullptr;
	ID3D12DescriptorHeap*				descriptorHeap = nullptr;

	UINT								rtvDescSize = 0;
};
