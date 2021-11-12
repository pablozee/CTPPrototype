#include "Fence.h"

namespace Fence
{
	void CreateFence(D3D12Global& d3d)
	{
		HRESULT hr = d3d.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d.fence));
		Utils::Validate(hr, L"Error: failed to create fence!");
#if NAME_D3D_RESOURCES
		d3d.fence->SetName(L"D3D12 Fence");
#endif

		d3d.fenceValues[d3d.frameIndex]++;

		// Create an event handle to use for frame synchronization
		d3d.fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		if (d3d.fenceEvent == nullptr)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			Utils::Validate(hr, L"Error: failed to create fence event!");
		}
	}
}