#pragma once

#include "Common.h"

struct ViewConstantBuffer
{
	DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT4 viewOriginAndTanHalfFovY = DirectX::XMFLOAT4(0, 0.f, 0.f, 0.f);
	DirectX::XMFLOAT2 resolution = DirectX::XMFLOAT2(1280, 720);
};

struct MaterialConstantBuffer
{
	DirectX::XMFLOAT4 resolution;
};

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
	ID3D12Resource*						vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW			vertexBufferView;
	ID3D12Resource*						indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW				indexBufferView;

	ID3D12Resource*						viewCB = nullptr;
	ViewConstantBuffer					viewCBData;
	UINT8*								viewCBStart = nullptr;

	ID3D12Resource*						materialCB = nullptr;
	MaterialConstantBuffer				materialCBData;
	UINT8*								materialCBStart = nullptr;

	ID3D12DescriptorHeap*				rtvHeap = nullptr;
	ID3D12DescriptorHeap*				descriptorHeap = nullptr;

	ID3D12Resource*						texture = nullptr;
	ID3D12Resource*						textureUploadResource = nullptr;


	UINT								rtvDescSize = 0;
};

static const D3D12_HEAP_PROPERTIES UploadHeapProperties =
{
	D3D12_HEAP_TYPE_UPLOAD,
	D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
	D3D12_MEMORY_POOL_UNKNOWN,
	0, 0
};

static const D3D12_HEAP_PROPERTIES DefaultHeapProperties =
{
	D3D12_HEAP_TYPE_DEFAULT,
	D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
	D3D12_MEMORY_POOL_UNKNOWN,
	0, 0
};