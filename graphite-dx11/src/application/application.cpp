#include "application.h"

Application::Application(HINSTANCE hInstance)
	: m_instanceHandle(hInstance) {}

Application::~Application() = default;

bool Application::Initialize(int width, int height)
{
	m_window = std::make_unique<Window>(m_instanceHandle);
	return m_window->Initialize(width, height);

	// m_window->SetResizeCallback([this](int w, int h) {
			// OnWindowResize(w, h);
		// });

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