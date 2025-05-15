#pragma once

#include <memory>
#include "window/window.h"

class Application
{
public:
	Application(HINSTANCE hInstance);
	~Application();

	bool Initialize(int width, int height);
	int Run();

private:
	std::unique_ptr<Window> m_window;
	HINSTANCE m_instanceHandle;

	void Update() { /* stub */ }
	void Render() { /* stub */ }

	void OnWindowResize(int width, int height) { /* stub */ }
};