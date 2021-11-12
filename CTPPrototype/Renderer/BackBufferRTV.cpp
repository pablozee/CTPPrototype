#include "BackBufferRTV.h"

namespace BackBufferRTV
{
	void CreateBackBufferRTV(D3D12Global& d3d, D3D12Resources& resources)
	{
		HRESULT hr;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

		rtvHandle = resources.rtvHeap->GetCPUDescriptorHandleForHeapStart();

		// Create a RTV for each back buffer
		for (UINT n = 0; n < 2; n++)
		{
			hr = d3d.swapChain->GetBuffer(n, IID_PPV_ARGS(&d3d.backBuffer[n]));
			Utils::Validate(hr, L"Error: failed to get swap chain buffer!");

			d3d.device->CreateRenderTargetView(d3d.backBuffer[n], nullptr, rtvHandle);

#if NAME_D3D_RESOURCES
			if (n == 0) d3d.backBuffer[n]->SetName(L"Back Buffer 0");
			else d3d.backBuffer[n]->SetName(L"Back Buffer 1");
#endif

			rtvHandle.ptr += resources.rtvDescSize;
		}
	}
}