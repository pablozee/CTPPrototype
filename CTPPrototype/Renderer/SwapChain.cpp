#include "SwapChain.h"

namespace SwapChain
{
	void CreateSwapChain(D3D12Global& d3d, HWND& hwnd)
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.BufferCount = 2;
		desc.Width = d3d.width;
		desc.Height = d3d.height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.SampleDesc.Count = 1;

		IDXGISwapChain1* swapChain;
		HRESULT hr = d3d.factory->CreateSwapChainForHwnd(d3d.cmdQueue, hwnd, &desc, nullptr, nullptr, &swapChain);
		Utils::Validate(hr, L"Error: failed to create swap chain!");

		hr = d3d.factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		Utils::Validate(hr, L"Error: failed to make window association!");

		hr = swapChain->QueryInterface(__uuidof(IDXGISwapChain3), reinterpret_cast<void**>(&d3d.swapChain));
		Utils::Validate(hr, L"Error: failed to cast swap chain!");

		SAFE_RELEASE(swapChain);
		d3d.frameIndex = d3d.swapChain->GetCurrentBackBufferIndex();

	}
}