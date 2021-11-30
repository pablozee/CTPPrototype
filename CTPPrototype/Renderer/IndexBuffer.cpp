#include "IndexBuffer.h"

namespace IndexBuffer
{
	void CreateIndexBuffer(D3D12Global& d3d, D3D12Resources& resources, std::vector<Model>& modelsVec)
	{
		size_t modelsIndicesSize = 0;

		for (int i = 0; i < modelsVec.size(); i++)
		{
			modelsIndicesSize += modelsVec[i].indices.size();
		}

		D3D12BufferCreateInfo info((UINT)modelsIndicesSize * sizeof(UINT), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		Buffer::CreateBuffer(d3d, info, &resources.indexBuffer);

#if NAME_D3D_RESOURCES
		resources.indexBuffer->SetName(L"Index Buffer");
#endif

		std::vector<uint32_t> allIndices;
		allIndices.reserve(modelsIndicesSize);

		for (int i = 0; i < modelsVec.size(); i++)
		{
			allIndices.insert(allIndices.end(), modelsVec[i].indices.begin(), modelsVec[i].indices.end());
		}

		// Copy the index data to the index buffer
		UINT8* pIndexDataBegin;
		D3D12_RANGE readRange = {};
		HRESULT hr = resources.indexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin));
		Utils::Validate(hr, L"Error: failed to map index buffer!");

		memcpy(pIndexDataBegin, allIndices.data(), info.size);
		resources.indexBuffer->Unmap(0, nullptr);

		// Initialize the index buffer view
		resources.indexBufferView.BufferLocation = resources.indexBuffer->GetGPUVirtualAddress();
		resources.indexBufferView.SizeInBytes = static_cast<UINT>(info.size);
		resources.indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}
}