#pragma once
#include <Windows.h>

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Initialize(HWND hwnd);
	void Update();
	void Draw();

	HWND GetHwnd() const { return m_Hwnd; }

private:
	void Shutdown();

	HWND m_Hwnd;
};