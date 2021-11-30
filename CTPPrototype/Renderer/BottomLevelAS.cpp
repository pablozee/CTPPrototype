#include "BottomLevelAS.h"
#include "Buffer.h"

/*
std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometryDesc;
geometryDesc.reserve(3);

geometryDesc[0].Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
geometryDesc[0].Triangles.VertexBuffer.StartAddress = m_D3DResources.vertexBuffer->GetGPUVirtualAddress();
geometryDesc[0].Triangles.VertexBuffer.StrideInBytes = m_D3DResources.vertexBufferView.StrideInBytes;
geometryDesc[0].Triangles.VertexCount = static_cast<uint32_t>(m_Model.vertices.size());
geometryDesc[0].Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
geometryDesc[0].Triangles.IndexBuffer = m_D3DResources.indexBuffer->GetGPUVirtualAddress();
geometryDesc[0].Triangles.IndexCount = static_cast<uint32_t>(m_Model.indices.size());
geometryDesc[0].Triangles.IndexFormat = m_D3DResources.indexBufferView.Format;
geometryDesc[0].Triangles.Transform3x4 = 0;
geometryDesc[0].Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

geometryDesc[1].Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
geometryDesc[1].Triangles.VertexBuffer.StartAddress = m_D3DResources.vertexBuffer->GetGPUVirtualAddress() + m_Model.vertices.size() * sizeof(Vertex);
geometryDesc[1].Triangles.VertexBuffer.StrideInBytes = m_D3DResources.vertexBufferView.StrideInBytes;
geometryDesc[1].Triangles.VertexCount = static_cast<uint32_t>(m_Model2.vertices.size());
geometryDesc[1].Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
geometryDesc[1].Triangles.IndexBuffer = m_D3DResources.indexBuffer->GetGPUVirtualAddress() + m_Model.indices.size() * sizeof(uint32_t);
geometryDesc[1].Triangles.IndexCount = static_cast<uint32_t>(m_Model2.indices.size());
geometryDesc[1].Triangles.IndexFormat = m_D3DResources.indexBufferView.Format;
geometryDesc[1].Triangles.Transform3x4 = 0;
geometryDesc[1].Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

*/
namespace BottomLevelAS
{
	void CreateBottomLevelAS(D3D12Global& d3d, DXRGlobal& dxr, D3D12Resources& resources, std::vector<Model> modelsVec)
	{
		// Describe the geometry that goes in the bottom acceleration structure(s)
		std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometryDescriptions = {};
		geometryDescriptions.reserve(modelsVec.size());

		D3D12_GPU_VIRTUAL_ADDRESS vertexBufferAddress = resources.vertexBuffer->GetGPUVirtualAddress();
		D3D12_GPU_VIRTUAL_ADDRESS indexBufferAddress = resources.indexBuffer->GetGPUVirtualAddress();


		
		for (int i = 0; i < modelsVec.size(); i++)
		{
			D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc;
			
			geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
			geometryDesc.Triangles.VertexBuffer.StartAddress = vertexBufferAddress;
			geometryDesc.Triangles.VertexBuffer.StrideInBytes = resources.vertexBufferView.StrideInBytes;
			geometryDesc.Triangles.VertexCount = static_cast<UINT>(modelsVec[i].vertices.size());
			geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			geometryDesc.Triangles.IndexBuffer = indexBufferAddress;
			geometryDesc.Triangles.IndexFormat = resources.indexBufferView.Format;
			geometryDesc.Triangles.IndexCount = static_cast<UINT>(modelsVec[i].indices.size());
			geometryDesc.Triangles.Transform3x4 = 0;
			geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

			geometryDescriptions.push_back(geometryDesc);

			vertexBufferAddress += modelsVec[i].vertices.size() * sizeof(Vertex);
			indexBufferAddress += modelsVec[i].indices.size() * sizeof(uint32_t);

		}

	

		

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

		// Get the size requirements for the BLAS buffers
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS ASInputs = {};
		ASInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		ASInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		ASInputs.pGeometryDescs = geometryDescriptions.data();
		ASInputs.NumDescs = geometryDescriptions.size();
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