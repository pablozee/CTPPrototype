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