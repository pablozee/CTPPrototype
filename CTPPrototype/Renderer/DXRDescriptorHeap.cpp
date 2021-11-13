#include "DXRDescriptorHeap.h"

namespace DXRDescriptorHeap
{
	void CreateDescriptorHeaps(D3D12Global& d3d, DXRGlobal& dxr, D3D12Resources& resources, const Model& model)
	{
		// Describe the CBV/SRV/UAV heap
		// Need 7 entries:
		// 1 CBV for the ViewCB
		// 1 CBV for the MaterialCB
		// 1 UAV for the RT output
		// 1 SRV for the Scene BVH
		// 1 SRV for the index buffer
		// 1 SRV for the vertex buffer
		// 1 SRV for the texture
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = 7;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		// Create the descriptor heap
		HRESULT hr = d3d.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&resources.descriptorHeap));
		Utils::Validate(hr, L"Error: failed to create DXR CBV/SRV/UAV descriptor heap!");

		// Get the descriptor heap handle and increment size
		D3D12_CPU_DESCRIPTOR_HANDLE handle = resources.descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		UINT handleIncrement = d3d.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
#if NAME_D3D_RESOURCES
		resources.descriptorHeap->SetName(L"DXR Descriptor Heap");
#endif

		// Create the ViewCB CBV
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.SizeInBytes = ALIGN(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, sizeof(resources.viewCBData));
		cbvDesc.BufferLocation = resources.viewCB->GetGPUVirtualAddress();

		d3d.device->CreateConstantBufferView(&cbvDesc, handle);

		// Create the MaterialCB CBV
		cbvDesc.SizeInBytes = ALIGN(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, sizeof(resources.materialCBData));
		cbvDesc.BufferLocation = resources.materialCB->GetGPUVirtualAddress();

		handle.ptr += handleIncrement;
		d3d.device->CreateConstantBufferView(&cbvDesc, handle);

		// Create the DXR output buffer UAV
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		handle.ptr += handleIncrement;
		d3d.device->CreateUnorderedAccessView(resources.DXROutput, nullptr, &uavDesc, handle);

		// Create the DXR Top Level Acceleration Structure SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.RaytracingAccelerationStructure.Location = dxr.TLAS.pResult->GetGPUVirtualAddress();

		handle.ptr += handleIncrement;
		d3d.device->CreateShaderResourceView(nullptr, &srvDesc, handle);

		// Create the index buffer SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC indexSRVDesc;
		indexSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		indexSRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		indexSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
		indexSRVDesc.Buffer.StructureByteStride = 0;
		indexSRVDesc.Buffer.FirstElement = 0;
		indexSRVDesc.Buffer.NumElements = (static_cast<UINT>(model.indices.size()) * sizeof(UINT)) / sizeof(float);
		indexSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		handle.ptr += handleIncrement;
		d3d.device->CreateShaderResourceView(resources.indexBuffer, &indexSRVDesc, handle);

		// Create the vertex buffer SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC vertexSRVDesc;
		vertexSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		vertexSRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		vertexSRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
		vertexSRVDesc.Buffer.StructureByteStride = 0;
		vertexSRVDesc.Buffer.FirstElement = 0;
		vertexSRVDesc.Buffer.NumElements = (static_cast<UINT>(model.vertices.size()) * sizeof(Vertex)) / sizeof(float);
		vertexSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		handle.ptr += handleIncrement;
		d3d.device->CreateShaderResourceView(resources.vertexBuffer, &vertexSRVDesc, handle);

		// Create the material texture SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC textureSRVDesc = {};
		textureSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		textureSRVDesc.Texture2D.MipLevels = 1;
		textureSRVDesc.Texture2D.MostDetailedMip = 0;
		textureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		handle.ptr += handleIncrement;
		d3d.device->CreateShaderResourceView(resources.texture, &textureSRVDesc, handle);
	}
}