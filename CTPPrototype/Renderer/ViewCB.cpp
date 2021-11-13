#include "ViewCB.h"
#include "ConstantBuffer.h"

namespace ViewCB
{
	void CreateViewCB(D3D12Global& d3d, D3D12Resources& resources)
	{
		ConstantBuffer::CreateConstantBuffer(d3d, &resources.viewCB, sizeof(ViewConstantBuffer));

#if NAME_D3D_RESOURCES
		resources.viewCB->SetName(L"View Constant Buffer");
#endif

		HRESULT hr = resources.viewCB->Map(0, nullptr, reinterpret_cast<void**>(&resources.viewCBStart));
		Utils::Validate(hr, L"Error: failed to map View constant buffer!");

		memcpy(resources.viewCBStart, &resources.viewCBData, sizeof(resources.viewCBData));

	}
}