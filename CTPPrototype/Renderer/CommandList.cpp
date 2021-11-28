#include "CommandList.h"
#include "Fence.h"

namespace CommandList
{
	void CreateCommandList(D3D12Global& d3d)
	{
		HRESULT hr = d3d.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3d.cmdAlloc[d3d.frameIndex], nullptr, IID_PPV_ARGS(&d3d.cmdList));
		hr = d3d.cmdList->Close();
		Utils::Validate(hr, L"Error: failed to create the command list!");

#if NAME_D3D_RESOURCES
		d3d.cmdList->SetName(L"D3D12 Command List");
#endif
	}

	void ResetCommandList(D3D12Global& d3d)
	{
		HRESULT hr = d3d.cmdAlloc[d3d.frameIndex]->Reset();
		Utils::Validate(hr, L"Error: failed to reset command allocator!");

		hr = d3d.cmdList->Reset(d3d.cmdAlloc[d3d.frameIndex], nullptr);
		Utils::Validate(hr, L"Error: failed to reset command list!");
	}

	void BuildCommandList(D3D12Global& d3d, DXRGlobal& dxr, D3D12Resources& resources)
	{
		D3D12_RESOURCE_BARRIER OutputBarriers[2] = {};
		D3D12_RESOURCE_BARRIER CounterBarriers[2] = {};
		D3D12_RESOURCE_BARRIER UAVBarriers[3] = {};

		// Transition the back buffer to a copy destination
		OutputBarriers[0].Transition.pResource = d3d.backBuffer[d3d.frameIndex];
		OutputBarriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		OutputBarriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		OutputBarriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		// Transition the DXR output buffer to a copy source
		OutputBarriers[1].Transition.pResource = resources.DXROutput;
		OutputBarriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		OutputBarriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		OutputBarriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		// Wait for the transitions to complete
		d3d.cmdList->ResourceBarrier(2, OutputBarriers);

		// Set the UAV/SRV/CBV and sampler heaps
		ID3D12DescriptorHeap* ppHeaps[] = { resources.descriptorHeap };
		d3d.cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// Dispatch rays
		D3D12_DISPATCH_RAYS_DESC desc = {};
		desc.RayGenerationShaderRecord.StartAddress = dxr.shaderTable->GetGPUVirtualAddress();
		desc.RayGenerationShaderRecord.SizeInBytes = dxr.shaderTableRecordSize;

		desc.MissShaderTable.StartAddress = dxr.shaderTable->GetGPUVirtualAddress() + dxr.shaderTableRecordSize;
		desc.MissShaderTable.SizeInBytes = dxr.shaderTableRecordSize;		// Only a single Miss program entry
		desc.MissShaderTable.StrideInBytes = dxr.shaderTableRecordSize;

		desc.HitGroupTable.StartAddress = dxr.shaderTable->GetGPUVirtualAddress() + (dxr.shaderTableRecordSize * 2);
		desc.HitGroupTable.SizeInBytes = dxr.shaderTableRecordSize;			// Only a single Hit program entry
		desc.HitGroupTable.StrideInBytes = dxr.shaderTableRecordSize;

		desc.Width = d3d.width;
		desc.Height = d3d.height;
		desc.Depth = 1;

		d3d.cmdList->SetPipelineState1(dxr.rtpso);
		d3d.cmdList->DispatchRays(&desc);

		// Transition DXR output to a copy source
		OutputBarriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		OutputBarriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

		// Wait for the transitions to complete
		d3d.cmdList->ResourceBarrier(1, &OutputBarriers[1]);

		// Copy the DXR output to the back buffer
		d3d.cmdList->CopyResource(d3d.backBuffer[d3d.frameIndex], resources.DXROutput);

		// Transition back buffer to present
		OutputBarriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		OutputBarriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		// Wait for the transitions to complete
		d3d.cmdList->ResourceBarrier(1, &OutputBarriers[0]);

		// Submit the command list and wait for the GPU to idle
		SubmitCommandList(d3d);
		Fence::WaitForGPU(d3d);
	}

	void SubmitCommandList(D3D12Global& d3d)
	{
		d3d.cmdList->Close();

		ID3D12CommandList* pGraphicsList = { d3d.cmdList };
		d3d.cmdQueue->ExecuteCommandLists(1, &pGraphicsList);
		d3d.fenceValues[d3d.frameIndex]++;
		d3d.cmdQueue->Signal(d3d.fence, d3d.fenceValues[d3d.frameIndex]);
	}
}