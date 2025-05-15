#pragma once

#include <memory>
#include "window/window.h"
#include "render/renderer.h"
#include "core/timer.h"

class Application
{
public:
	Application(HINSTANCE hInstance);
	~Application();

	bool Initialize(int width, int height);
	int Run();

private:
	std::unique_ptr<Window> m_window;
	std::unique_ptr<Renderer> m_renderer;
	Timer m_timer;
	HINSTANCE m_instanceHandle;


	void Update(float dt);
	void Render(const FrameRenderContext& context);
};