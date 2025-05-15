#pragma once

#include <Windows.h>
#include <functional>

class Window
{
public:
	using ResizeCallback = std::function<void(int newWidth, int newHeight)>;

	Window(HINSTANCE hInstance);
	~Window();

	bool Initialize(int width, int height);
	bool ProcessMessages();


	void SetResizeCallback(ResizeCallback callback) { m_resizeCallback = std::move(callback); }

	HWND GetHandle() const { return m_hWnd; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	RECT GetClientRect() const;

private:

	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam
	);

	LRESULT HandleMessage(
		HWND hWnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam
	);

	bool RegisterWindowClass();

	HINSTANCE m_instanceHandle;
	HWND m_hWnd;

	ResizeCallback m_resizeCallback;

	int m_width = 0;
	int m_height = 0;
};