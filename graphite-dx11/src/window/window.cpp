#include "window.h"
#include "utils/errors.h"

#define LOG_WINDOW_INFO(msg) window::LogInfo("WINDOW", msg)

static constexpr LPCWSTR WINDOW_CLASS_NAME = L"GraphiteWindowClass";
static constexpr LPCWSTR WINDOW_TITLE = L"Graphite-DX11";

Window::Window(HINSTANCE hInstance)
	: m_instanceHandle(hInstance), m_hWnd(nullptr) {}

Window::~Window()
{
	if (m_hWnd)
	{
		LOG_WINDOW_INFO("Destroying window");
		DestroyWindow(m_hWnd);
		UnregisterClass(WINDOW_CLASS_NAME, m_instanceHandle);
	}
}

bool Window::RegisterWindowClass()
{
	// check if window already registered
	// even tho it won't be
	if (GetClassInfoEx(m_instanceHandle, WINDOW_CLASS_NAME, nullptr))
		return true;

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = m_instanceHandle;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	return RegisterClassEx(&wc);
}

bool Window::Initialize(int width, int height)
{
	m_width = width;
	m_height = height;

	if (!RegisterWindowClass())
	{
		window::LogLastError("RegisterWindowClass");
		return false;
	}

	LOG_WINDOW_INFO("Window class registered");

	m_hWnd = CreateWindowEx(
		0,
		WINDOW_CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		nullptr,
		nullptr,
		m_instanceHandle,
		this
	);

	if (!m_hWnd)
	{
		window::LogLastError("CreateWindowEx");
		return false;
	}

	LOG_WINDOW_INFO("Window created");

	ShowWindow(m_hWnd, SW_SHOW);
	return true;
}

RECT Window::GetClientRect() const
{
	RECT rect = {};
	::GetClientRect(m_hWnd, &rect);
	return rect;
}

bool Window::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			LOG_WINDOW_INFO("WM_QUIT received");
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

LRESULT CALLBACK Window::WindowProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	Window* pThis = nullptr;

	if (msg == WM_NCCREATE)
	{
		auto createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<Window*>(createStruct->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	} else {
		pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pThis)
	{
		return pThis->HandleMessage(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT  Window::HandleMessage(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		m_width = LOWORD(lParam);
		m_height = HIWORD(lParam);

		if (m_resizeCallback)
		{
			m_resizeCallback(m_width, m_height);
		}
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}