#include "Renderer.h"
#include "Utils.h"
#include "Device.h"
#include "CommandQueue.h"
#include "CommandAllocator.h"
#include "Fence.h"
#include "SwapChain.h"
#include "CommandList.h"
#include "DescriptorHeap.h"
#include "BackBufferRTV.h"

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
	Shutdown();
}

void Renderer::Initialize(HWND hwnd)
{
	m_Hwnd = hwnd;

	d3d.width = 1280;
	d3d.height = 960;
	d3d.vsync = true;

	Utils::LoadModel("Models/cinema.obj", model, material);

	D3DShaders::Init_Shader_Compiler(shaderCompiler);

	// Initialize D3D12
	Device::CreateDevice(d3d);

	CommandQueue::CreateCommandQueue(d3d);
	CommandAllocator::CreateCommandAllocator(d3d);

	Fence::CreateFence(d3d);

	SwapChain::CreateSwapChain(d3d, hwnd);

	CommandList::CreateCommandList(d3d);
	CommandList::ResetCommandList(d3d);

	// Create common resources
	DescriptorHeap::CreateDescriptorHeaps(d3d, resources);

	BackBufferRTV::CreateBackBufferRTV(d3d, resources);
}

void Renderer::Update()
{

}

void Renderer::Draw()
{

}

void Renderer::Shutdown()
{

}