#include "application.h"
#include "utils/errors.h"

Application::Application(HINSTANCE hInstance)
	: m_instanceHandle(hInstance) {}

Application::~Application() = default;

bool Application::Initialize(int width, int height)
{
	m_window = std::make_unique<Window>(m_instanceHandle);
	if (!m_window->Initialize(width, height))
	{
		window::LogLastError("Window::Initialize");
		return false;
	}

	m_window->SetResizeCallback([this](int w, int h) {
			OnWindowResize(w, h);
		});

		//
		// TO-DO
			// initialize renderer here
			// pass window handle and window size
			//

	return true;
}

int Application::Run()
{
	while (true)
	{
		if (!m_window->ProcessMessages())
			break;

		Update();
		Render();
	}

	return 0;
}