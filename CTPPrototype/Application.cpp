#include "Application.h"
#include <assert.h>
#include <Windows.h>

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

Application* Application::s_Instance = nullptr;

Application::Application()
{
	assert(!s_Instance);
	s_Instance = this;

	m_Window = std::unique_ptr<Window>(new Window());
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
};

Application::~Application()
{

}

void Application::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
}

int Application::Run()
{
	m_Timer.Reset();

	while (m_Running)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}

		m_Timer.Tick();

		CalculateFrameStatistics();
		m_Window->GetRenderer()->Update();
		m_Window->GetRenderer()->Draw();
	}

	return 0;
}

void Application::CalculateFrameStatistics()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((m_Timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = m_MainWindowCaption +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(m_Window->GetRenderer()->GetHwnd(), windowText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

bool Application::OnWindowClose(WindowCloseEvent& event)
{
	m_Running = false;
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent& event)
{
	// TODO Call Renderer resize code here
	return true;
}