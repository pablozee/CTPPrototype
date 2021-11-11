#include "CommandAllocator.h"

namespace CommandAllocator
{
	void CreateCommandAllocator(D3D12Global& d3d)
	{
		for (UINT n = 0; n < 2; n++)
		{
			HRESULT hr = d3d.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3d.cmdAlloc[n]));
			Utils::Validate(hr, L"Error: failed to create the command allocator!");

#if NAME_D3D_RESOURCES
			if (n == 0) d3d.cmdAlloc[n]->SetName(L"D3D12 Command Allocator 0");
			else d3d.cmdAlloc[n]->SetName(L"D3D12 Command Allocator 1");
#endif
		}
	}
}