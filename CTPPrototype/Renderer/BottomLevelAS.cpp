#include "BottomLevelAS.h"
#include "Buffer.h"

namespace BottomLevelAS
{
	void CreateBottomLevelAS(D3D12Global& d3d, DXRGlobal& dxr, D3D12Resources& resources, Model& model)
	{
		// Describe the geometry that goes in the bottom acceleration structure(s)
		D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc;
		geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		geometryDesc.Triangles.VertexBuffer.StartAddress = resources.vertexBuffer->GetGPUVirtualAddress();
		geometryDesc.Triangles.VertexBuffer.StrideInBytes = resources.vertexBufferView.StrideInBytes;
		geometryDesc.Triangles.VertexCount = static_cast<UINT>(model.vertices.size());
		geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		geometryDesc.Triangles.IndexBuffer = resources.indexBuffer->GetGPUVirtualAddress();
		geometryDesc.Triangles.IndexFormat = resources.indexBufferView.Format;
		geometryDesc.Triangles.IndexCount = static_cast<UINT>(model.indices.size());
		geometryDesc.Triangles.Transform3x4 = 0;
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

		// Get the size requirements for the BLAS buffers
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS ASInputs = {};
		ASInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		ASInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		ASInputs.pGeometryDescs = &geometryDesc;
		ASInputs.NumDescs = 1;
		ASInputs.Flags = buildFlags;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO ASPreBuildInfo = {};
		d3d.device->GetRaytracingAccelerationStructurePrebuildInfo(&ASInputs, &ASPreBuildInfo);

		ASPreBuildInfo.ScratchDataSizeInBytes = ALIGN(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT, ASPreBuildInfo.ScratchDataSizeInBytes);
		ASPreBuildInfo.ResultDataMaxSizeInBytes = ALIGN(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT, ASPreBuildInfo.ResultDataMaxSizeInBytes);

		// Create the BLAS scratch buffer
		D3D12BufferCreateInfo bufferInfo(ASPreBuildInfo.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		bufferInfo.alignment = max(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
		Buffer::CreateBuffer(d3d, bufferInfo, &dxr.BLAS.pScratch);

#if NAME_D3D_RESOURCES
		dxr.BLAS.pScratch->SetName(L"DXR BLAS Scratch");
#endif

		// Create the BLAS buffer
		bufferInfo.size = ASPreBuildInfo.ResultDataMaxSizeInBytes;
		bufferInfo.state = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		Buffer::CreateBuffer(d3d, bufferInfo, &dxr.BLAS.pResult);

#if NAME_D3D_RESOURCES
		dxr.BLAS.pResult->SetName(L"DXR BLAS");
#endif

		// Describe and build the bottom level acceleration structure
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
		buildDesc.Inputs = ASInputs;
		buildDesc.ScratchAccelerationStructureData = dxr.BLAS.pScratch->GetGPUVirtualAddress();
		buildDesc.DestAccelerationStructureData = dxr.BLAS.pResult->GetGPUVirtualAddress();

		d3d.cmdList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);

		// Wait for the BLAS build to complete
		D3D12_RESOURCE_BARRIER uavBarrier;
		uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		uavBarrier.UAV.pResource = dxr.BLAS.pResult;
		uavBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		d3d.cmdList->ResourceBarrier(1, &uavBarrier);
	}
}