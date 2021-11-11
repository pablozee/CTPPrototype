#pragma once
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Window.h"
#include "GameTimer.h"


class Application
{
public:
	Application();
	virtual ~Application();

	int Run();

	void OnEvent(Event& event);

	inline static Application& Get() { return *s_Instance; }

	inline Window& GetWindow() { return *m_Window; }


private:
	bool m_Running = true;
	bool OnWindowClose(WindowCloseEvent& event);
	bool OnWindowResize(WindowResizeEvent& event);

	void CalculateFrameStatistics();

	std::unique_ptr<Window> m_Window;
	static Application* s_Instance;

	GameTimer m_Timer;
	std::wstring m_MainWindowCaption = L"CTP Prototype Raytracer";
	std::wstring m_WindowText;
};