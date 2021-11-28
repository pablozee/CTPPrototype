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

struct D3D12ShaderInfo
{
	LPCWSTR		filename = nullptr;
	LPCWSTR		entryPoint = nullptr;
	LPCWSTR		targetProfile = nullptr;
	LPCWSTR* arguments = nullptr;
	DxcDefine* defines = nullptr;
	UINT32		argCount = 0;
	UINT32		defineCount = 0;

	D3D12ShaderInfo() {}
	D3D12ShaderInfo(LPCWSTR inFilename, LPCWSTR inEntryPoint, LPCWSTR inProfile)
	{
		filename = inFilename;
		entryPoint = inEntryPoint;
		targetProfile = inProfile;
	}
};

struct D3D12ShaderCompilerInfo
{
	dxc::DxcDllSupport		DxcDllHelper;
	IDxcCompiler* compiler = nullptr;
	IDxcLibrary* library = nullptr;
};

struct AccelerationStructureBuffer
{
	ID3D12Resource* pScratch = nullptr;
	ID3D12Resource* pResult = nullptr;
	ID3D12Resource* pInstanceDesc = nullptr;	// only used in top-level AS
};

struct RtProgram
{
	D3D12ShaderInfo			info = {};
	IDxcBlob* blob = nullptr;
	ID3D12RootSignature* pRootSignature = nullptr;

	D3D12_DXIL_LIBRARY_DESC	dxilLibDesc;
	D3D12_EXPORT_DESC		exportDesc;
	D3D12_STATE_SUBOBJECT	subobject;
	std::wstring			exportName;

	RtProgram()
	{
		exportDesc.ExportToRename = nullptr;
	}

	RtProgram(D3D12ShaderInfo shaderInfo)
	{
		info = shaderInfo;
		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
		exportName = shaderInfo.entryPoint;
		exportDesc.ExportToRename = nullptr;
		exportDesc.Flags = D3D12_EXPORT_FLAG_NONE;
	}

	void SetBytecode()
	{
		exportDesc.Name = exportName.c_str();

		dxilLibDesc.NumExports = 1;
		dxilLibDesc.pExports = &exportDesc;
		dxilLibDesc.DXILLibrary.BytecodeLength = blob->GetBufferSize();
		dxilLibDesc.DXILLibrary.pShaderBytecode = blob->GetBufferPointer();

		subobject.pDesc = &dxilLibDesc;
	}

};

struct HitProgram
{
	RtProgram ahs;
	RtProgram chs;

	std::wstring exportName;
	D3D12_HIT_GROUP_DESC desc = {};
	D3D12_STATE_SUBOBJECT subobject = {};

	HitProgram() {}
	HitProgram(LPCWSTR name) : exportName(name)
	{
		desc = {};
		desc.HitGroupExport = exportName.c_str();
		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
		subobject.pDesc = &desc;
	}

	void SetExports(bool anyHit)
	{
		desc.HitGroupExport = exportName.c_str();
		if (anyHit) desc.AnyHitShaderImport = ahs.exportDesc.Name;
		desc.ClosestHitShaderImport = chs.exportDesc.Name;
	}

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
	ID3D12Resource*						DXROutput;

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

	float											translationOffset = 0;
	float											rotationOffset = 0;
	DirectX::XMFLOAT3								eyeAngle;
	DirectX::XMFLOAT3								eyePosition;
};

struct DXRGlobal
{
	AccelerationStructureBuffer						TLAS;
	AccelerationStructureBuffer						BLAS;
	uint64_t										tlasSize;

	ID3D12Resource* shaderTable = nullptr;
	uint32_t										shaderTableRecordSize = 0;

	RtProgram										rgs;
	RtProgram										miss;
	HitProgram										hit;

	ID3D12StateObject* rtpso = nullptr;
	ID3D12StateObjectProperties* rtpsoInfo = nullptr;
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