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

	void WaitForGPU(D3D12Global& d3d)
	{
		// Schedule a signal command in the queue
		HRESULT hr = d3d.cmdQueue->Signal(d3d.fence, d3d.fenceValues[d3d.frameIndex]);
		Utils::Validate(hr, L"Error: failed to signal fence!");

		// Wait until the fence has been processed
		hr = d3d.fence->SetEventOnCompletion(d3d.fenceValues[d3d.frameIndex], d3d.fenceEvent);
		Utils::Validate(hr, L"Error: failed to set fence event!");

		WaitForSingleObjectEx(d3d.fenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame
		d3d.fenceValues[d3d.frameIndex]++;
	}

	void MoveToNextFrame(D3D12Global& d3d)
	{
		// Schedule a Signal command in the queue
		const UINT64 currentFenceValue = d3d.fenceValues[d3d.frameIndex];
		HRESULT hr = d3d.cmdQueue->Signal(d3d.fence, currentFenceValue);
		Utils::Validate(hr, L"Error: failed to signal command queue!");

		// Update the frame index
		d3d.frameIndex = d3d.swapChain->GetCurrentBackBufferIndex();

		// If the next frame is not ready to be rendered yet, wait until it is
		if (d3d.fence->GetCompletedValue() < d3d.fenceValues[d3d.frameIndex])
		{
			hr = d3d.fence->SetEventOnCompletion(d3d.fenceValues[d3d.frameIndex], d3d.fenceEvent);
			Utils::Validate(hr, L"Error: failed to set fence value!");

			WaitForSingleObjectEx(d3d.fenceEvent, INFINITE, FALSE);
		}

		// Set the fence value for the next frame
		d3d.fenceValues[d3d.frameIndex] = currentFenceValue + 1;
	}
}