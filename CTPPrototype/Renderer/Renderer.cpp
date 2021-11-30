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
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "ViewCB.h"
#include "MaterialCB.h"
#include "BottomLevelAS.h"
#include "TopLevelAS.h"
#include "DXROutput.h"
#include "DXRDescriptorHeap.h"
#include "RayGen.h"
#include "Miss.h"
#include "ClosestHit.h"
#include "PSO.h"
#include "ShaderTable.h"

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

	Utils::LoadModel("Models/PortalSceneJoined.obj", models, materials);
	Utils::LoadModel("Models/Cube.obj", models, materials);

	D3DShaders::InitShaderCompiler(shaderCompiler);

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
	
	VertexBuffer::CreateVertexBuffer(d3d, resources, models);
	IndexBuffer::CreateIndexBuffer(d3d, resources, models);

//	Texture::CreateTexture(d3d, resources, materials[0]);

	ViewCB::CreateViewCB(d3d, resources);
	MaterialCB::CreateMaterialCB(d3d, resources, materials);


	// Create DXR Resources
	BottomLevelAS::CreateBottomLevelAS(d3d, dxr, resources, models);
	TopLevelAS::CreateTopLevelAS(d3d, dxr, resources, models);

	DXROutput::CreateDXROutput(d3d, resources);

	DXRDescriptorHeap::CreateDescriptorHeaps(d3d, dxr, resources, models, materials);
	
	RayGen::CreateRayGenProgram(d3d, dxr, shaderCompiler);
	Miss::CreateMissProgram(d3d, dxr, shaderCompiler);
	ClosestHit::CreateClosestHitProgram(d3d, dxr, shaderCompiler);

	PSO::CreatePipelineStateObject(d3d, dxr);
	
	ShaderTable::CreateShaderTable(d3d, dxr, resources);

	// Close and execute command list
	d3d.cmdList->Close();
	ID3D12CommandList* pGraphicsList = { d3d.cmdList };
	d3d.cmdQueue->ExecuteCommandLists(1, &pGraphicsList);

	Fence::WaitForGPU(d3d);

	CommandList::ResetCommandList(d3d);
}

void Renderer::Update()
{
	ViewCB::UpdateViewCB(d3d, resources);
}

void Renderer::Draw()
{
	CommandList::BuildCommandList(d3d, dxr, resources);
	
	SwapChain::Present(d3d);
	
	Fence::MoveToNextFrame(d3d);
	
	CommandList::ResetCommandList(d3d);
}

void Renderer::Shutdown()
{

}