#include "MaterialCB.h"
#include "ConstantBuffer.h"

namespace MaterialCB
{
	void CreateMaterialCB(D3D12Global& d3d, D3D12Resources& resources, const Material& material)
	{
		ConstantBuffer::CreateConstantBuffer(d3d, &resources.materialCB, sizeof(MaterialConstantBuffer));

#if NAME_D3D_RESOURCES
		resources.materialCB->SetName(L"Material Constant Buffer");
#endif

		resources.materialCBData.resolution = XMFLOAT4(material.textureResolution, 0.f, 0.f, 0.f);

		HRESULT hr = resources.materialCB->Map(0, nullptr, reinterpret_cast<void**>(&resources.materialCBStart));
		Utils::Validate(hr, L"Error: failed to map Material constant buffer!");

		memcpy(resources.materialCBStart, &resources.materialCBData, sizeof(resources.materialCBData));
	}
}