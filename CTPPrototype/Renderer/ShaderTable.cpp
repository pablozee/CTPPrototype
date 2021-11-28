#include "ShaderTable.h"
#include "Buffer.h"

namespace ShaderTable
{
	void CreateShaderTable(D3D12Global& d3d, DXRGlobal& dxr, D3D12Resources& resources)
	{
		/*
The Shader Table layout is as follows:
	Entry 0 - Ray Generation shader
	Entry 1 - Miss shader
	Entry 2 - Closest Hit shader
All shader records in the Shader Table must have the same size, so shader record size will be based on the largest required entry.
The ray generation program requires the largest entry:
	32 bytes - D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES
  +  8 bytes - a CBV/SRV/UAV descriptor table pointer (64-bits)
  = 40 bytes ->> aligns to 64 bytes
The entry size must be aligned up to D3D12_RAYTRACING_SHADER_BINDING_TABLE_RECORD_BYTE_ALIGNMENT
*/

		uint32_t shaderIdSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		uint32_t shaderTableSize = 0;

		dxr.shaderTableRecordSize = shaderIdSize;
		dxr.shaderTableRecordSize += 8;							// CBV/SRV/UAV descriptor table
		dxr.shaderTableRecordSize = ALIGN(D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT, dxr.shaderTableRecordSize);

		shaderTableSize = (dxr.shaderTableRecordSize * 3);		// 3 shader records in the table
		shaderTableSize = ALIGN(D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT, shaderTableSize);

		// Create the shader table buffer
		D3D12BufferCreateInfo bufferInfo(shaderTableSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		Buffer::CreateBuffer(d3d, bufferInfo, &dxr.shaderTable);

#if NAME_D3D_RESOURCES
		dxr.shaderTable->SetName(L"DXR Shader Table");
#endif

		// Map the buffer
		uint8_t* pData;
		HRESULT hr = dxr.shaderTable->Map(0, nullptr, (void**)&pData);
		Utils::Validate(hr, L"Error: failed to map shader table!");

		// Shader Record 0 - Ray Generation program and local root parameter data (descriptor table with constant buffer and IB/VB pointers)
		memcpy(pData, dxr.rtpsoInfo->GetShaderIdentifier(L"RayGen_12"), shaderIdSize);

		// Set the root parameter data. Point to start of descriptor heap.
		*reinterpret_cast<D3D12_GPU_DESCRIPTOR_HANDLE*>(pData + shaderIdSize) = resources.descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		// Shader Record 1 - Miss program (no local root arguments to set)
		pData += dxr.shaderTableRecordSize;
		memcpy(pData, dxr.rtpsoInfo->GetShaderIdentifier(L"Miss_5"), shaderIdSize);

		// Shader Record 2 - Closest Hit program and local root parameter data (descriptor table with constant buffer and IB/VB pointers)
		pData += dxr.shaderTableRecordSize;
		memcpy(pData, dxr.rtpsoInfo->GetShaderIdentifier(L"HitGroup"), shaderIdSize);

		// Set the root parameter data. Point to start of descriptor heap.
		*reinterpret_cast<D3D12_GPU_DESCRIPTOR_HANDLE*>(pData + shaderIdSize) = resources.descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		// Unmap
		dxr.shaderTable->Unmap(0, nullptr);
	}
}