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

	void UpdateViewCB(D3D12Global& d3d, D3D12Resources& resources)
	{
		const float rotationSpeed = 0.005f;
		XMMATRIX view, invView;
		XMFLOAT3 eye, focus, up;
		float aspect, fov;

		resources.eyeAngle.x += rotationSpeed;

#if _DEBUG
		float x = 2.f * cosf(resources.eyeAngle.x);
		float y = 0.f;
		float z = 2.25f + 2.f * sinf(resources.eyeAngle.x);

		focus = XMFLOAT3(0.f, 0.f, 0.f);
#else
		float x = 8.f * cosf(resources.eyeAngle.x);
		float y = 1.5f + 1.5f * cosf(resources.eyeAngle.x);
		float z = 8.f + 2.25f * sinf(resources.eyeAngle.x);
		focus = XMFLOAT3(0.f, 1.75f, 0.f);
#endif
	
		eye = XMFLOAT3(x, y + 3, z + 10);
		up = XMFLOAT3(0.f, 1.f, 0.f);

		aspect = (float)d3d.width / (float)d3d.height;
		fov = 65.f * (XM_PI / 180.f);							// convert to radians

		resources.rotationOffset += rotationSpeed;

		view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&focus), XMLoadFloat3(&up));
		invView = XMMatrixInverse(NULL, view);

		resources.viewCBData.view = XMMatrixTranspose(invView);
		resources.viewCBData.viewOriginAndTanHalfFovY = XMFLOAT4(eye.x, eye.y, eye.z, tanf(fov * 0.5f));
		resources.viewCBData.resolution = XMFLOAT2((float)d3d.width, (float)d3d.height);
		memcpy(resources.viewCBStart, &resources.viewCBData, sizeof(resources.viewCBData));
	}
}