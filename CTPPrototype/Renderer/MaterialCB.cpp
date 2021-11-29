#include "MaterialCB.h"
#include "Buffer.h"

namespace MaterialCB
{
	void CreateMaterialCB(D3D12Global& d3d, D3D12Resources& resources, const std::vector<Material>& materials)
	{
		D3D12BufferCreateInfo info((UINT)materials.size() * sizeof(MatCB), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		Buffer::CreateBuffer(d3d, info, &resources.materialBuffer);

#if NAME_D3D_RESOURCES
		resources.materialBuffer->SetName(L"Material Constant Buffer");
#endif

		std::vector<MatCB> newMats;
		newMats.reserve(materials.size());
		
		for (int i = 0; i < materials.size(); i++)
		{
			MatCB newMat;
			newMat.resolution = XMFLOAT4(materials[i].textureResolution, 0.f, 0.f, 0.f);
			newMat.diffuse = materials[i].diffuse;
			newMat.useTex = 0;

			newMats.push_back(newMat);
		}

		UINT8* pMaterialDataBegin;
		D3D12_RANGE readRange = {};
		HRESULT hr = resources.materialBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pMaterialDataBegin));
		Utils::Validate(hr, L"Error: failed to map vertex buffer!");

		memcpy(pMaterialDataBegin, newMats.data(), info.size);
		resources.materialBuffer->Unmap(0, nullptr);

		/*
		HRESULT hr = resources.materialCB->Map(0, nullptr, reinterpret_cast<void**>(&resources.materialCBStart));
		Utils::Validate(hr, L"Error: failed to map Material constant buffer!");

		memcpy(resources.materialCBStart, &resources.materialCBData, sizeof(resources.materialCBData));
		*/
	}
}