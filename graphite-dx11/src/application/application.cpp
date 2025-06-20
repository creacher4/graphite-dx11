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

	m_timer.Reset();

	return true;
}

int Application::Run()
{
	while (true)
	{
		if (!m_window->ProcessMessages())
			break;

		float dt = m_timer.GetDeltaTime();

		FrameRenderContext frameContext;
		frameContext.deltaTime = dt;
		frameContext.windowWidth = m_window->GetWidth();
		frameContext.windowHeight = m_window->GetHeight();

		Update(dt);
		Render(frameContext);
	}

	return 0;
}

void Application::Update(float dt)
{
	/// stub
}

void Application::Render(const FrameRenderContext& context)
{
	if (m_renderer)
	{
		m_renderer->RenderFrame(context);
	}
}