#include "VertexBuffer.h"

namespace VertexBuffer
{
	void CreateVertexBuffer(D3D12Global& d3d, D3D12Resources& resources, std::vector<Model>& modelsVec)
	{
		size_t modelsVerticesSize = 0;

		for (int i = 0; i < modelsVec.size(); i++)
		{
			modelsVerticesSize += modelsVec[i].vertices.size();
		}
		
		D3D12BufferCreateInfo info(((UINT)modelsVerticesSize * sizeof(Vertex)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		Buffer::CreateBuffer(d3d, info, &resources.vertexBuffer);

#if NAME_D3D_RESOURCES
		resources.vertexBuffer->SetName(L"Vertex Buffer");
#endif

		std::vector<Vertex> allVertices;
		allVertices.reserve(modelsVerticesSize);
		
		for (int i = 0; i < modelsVec.size(); i++)
		{
			allVertices.insert(allVertices.end(), modelsVec[i].vertices.begin(), modelsVec[i].vertices.end());
		}

		UINT8* pVertexDataBegin;
		D3D12_RANGE readRange = {};
		HRESULT hr = resources.vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		Utils::Validate(hr, L"Error: failed to map vertex buffer!");

		memcpy(pVertexDataBegin, allVertices.data(), info.size);
		resources.vertexBuffer->Unmap(0, nullptr);

		resources.vertexBufferView.BufferLocation = resources.vertexBuffer->GetGPUVirtualAddress();
		resources.vertexBufferView.StrideInBytes = sizeof(Vertex);
		resources.vertexBufferView.SizeInBytes = static_cast<UINT>(info.size);
	}
}