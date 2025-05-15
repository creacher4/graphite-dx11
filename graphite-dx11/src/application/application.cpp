#include "application.h"
#include "utils/errors.h"
#include "render/dx11/d3d11_renderer.h"

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
		if (m_renderer)
		{
			m_renderer->Resize(w, h);
		}
	});

	m_renderer = std::make_unique<D3D11Renderer>();
	if (!m_renderer->Initialize(m_window->GetHandle(), width, height))
	{
		window::LogLastError("Renderer::Initialize");
		return false;
	}

		//
		// TO-DO
			// initialize renderer here
			// pass window handle and window size
			//

	return true;
}

int Application::Run()
{
	// TODO
	// ADD TIMER SYSTEM
	FrameRenderContext frameContext;

	while (true)
	{
		if (!m_window->ProcessMessages())
			break;

		frameContext.deltaTime = 1.0f / 60.0f; // stub
		frameContext.windowWidth = m_window->GetWidth();
		frameContext.windowHeight = m_window->GetHeight();

		if (m_renderer)
		{
			m_renderer->RenderFrame(frameContext);
		}

		// future: Update();
	}

	return 0;
}