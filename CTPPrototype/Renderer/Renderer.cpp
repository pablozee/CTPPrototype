#include "Renderer.h"

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