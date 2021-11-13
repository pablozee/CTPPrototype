#include "DXROutput.h"

namespace DXROutput
{
	void CreateDXROutput(D3D12Global& d3d, D3D12Resources& resources)
	{
		// Describe the DXR output resource (texture)
		// Dimensions and format should match the swapchain
		// Initialize as a copy source, since we will copy this buffer's contents to the swapchain
		D3D12_RESOURCE_DESC desc = {};
		desc.DepthOrArraySize = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		desc.Width = d3d.width;
		desc.Height = d3d.height;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		// Create the buffer resource
		HRESULT hr = d3d.device->CreateCommittedResource(&DefaultHeapProperties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr, IID_PPV_ARGS(&resources.DXROutput));
		Utils::Validate(hr, L"Error: failed to create DXR output buffer!");

#if NAME_D3D_RESOURCES
		resources.DXROutput->SetName(L"DXR Output Buffer");
#endif
	}
}