#include "DXRRootSignature.h"

namespace DXRRootSignature
{
	ID3D12RootSignature* CreateRootSignature(D3D12Global& d3d, const D3D12_ROOT_SIGNATURE_DESC& desc)
	{
		ID3DBlob* sig;
		ID3DBlob* error;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &error);
		Utils::Validate(hr, L"Error: failed to serialize root signature!");

		ID3D12RootSignature* pRootSig;
		hr = d3d.device->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&pRootSig));
		Utils::Validate(hr, L"Error: failed to create root signature!");

		SAFE_RELEASE(sig);
		SAFE_RELEASE(error);
		return pRootSig;
	}
}