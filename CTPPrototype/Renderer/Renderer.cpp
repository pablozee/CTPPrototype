#include "Renderer.h"
#include "Utils.h"
#include "Device.h"
#include "CommandQueue.h"
#include "CommandAllocator.h"

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

	Device::CreateDevice(d3d);

	CommandQueue::CreateCommandQueue(d3d);
	CommandAllocator::CreateCommandAllocator(d3d);
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