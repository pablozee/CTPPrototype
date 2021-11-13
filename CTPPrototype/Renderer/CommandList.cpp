#include "CommandList.h"

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
}