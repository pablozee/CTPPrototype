#include "Device.h"
#include "Common.h"

namespace Device
{
	void CreateDevice(D3D12Global& d3d)
	{
#if defined(_DEBUG)
		// Enable the D3D12 debug layer.
		{
			ID3D12Debug* debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
			}
		}
#endif

		// Create a DXGI Factory
		HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&d3d.factory));
		Utils::Validate(hr, L"Error: failed to create DXGI factory!");

		// Create the device
		d3d.adapter = nullptr;
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != d3d.factory->EnumAdapters1(adapterIndex, &d3d.adapter); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 adapterDesc;
			d3d.adapter->GetDesc1(&adapterDesc);

			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;	// Don't select the Basic Render Driver adapter.
			}

			if (SUCCEEDED(D3D12CreateDevice(d3d.adapter, D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device5), (void**)&d3d.device)))
			{
				// Check if the device supports ray tracing.
				D3D12_FEATURE_DATA_D3D12_OPTIONS5 features = {};
				HRESULT hr = d3d.device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &features, sizeof(features));
				if (FAILED(hr) || features.RaytracingTier < D3D12_RAYTRACING_TIER_1_0)
				{
					SAFE_RELEASE(d3d.device);
					d3d.device = nullptr;
					continue;
				}

#if NAME_D3D_RESOURCES
				d3d.device->SetName(L"DXR Enabled Device");
				printf("Running on DXGI Adapter %S\n", adapterDesc.Description);
#endif
				break;
			}

			if (d3d.device == nullptr)
			{
				// Didn't find a device that supports ray tracing.
				Utils::Validate(E_FAIL, L"Error: failed to create ray tracing device!");
			}
		}
	}
}