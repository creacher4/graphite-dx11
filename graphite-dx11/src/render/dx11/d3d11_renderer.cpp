#include "d3d11_renderer.h"

bool D3D11Renderer::Initialize(HWND hwnd, int width, int height)
{
	// TO-DO:
	// initialize d3d11 device, swapchain, rtvs etc.

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hwnd;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.Windowed = TRUE;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createFlags = 0;
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapDesc,
		&m_swapChain,
		&m_device,
		&featureLevel,
		&m_context
	);

	if (FAILED(hr))
	{
		return false;
	}

	// get back buffer and create rtv
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_rtv);
	if (FAILED(hr))
	{
		return false;
	}

	// set viewport
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_context->RSSetViewports(1, &viewport);

	return true;
}

void D3D11Renderer::RenderFrame(const FrameRenderContext& context)
{
	// dummy frame
	float clearColor[4] = { 0.1f, 0.1f, 0.3f, 1.0f };
	m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), nullptr);
	m_context->ClearRenderTargetView(m_rtv.Get(), clearColor);

	m_swapChain->Present(1, 0);
}

void D3D11Renderer::Resize(int width, int height)
{
	if (m_context) m_context->OMSetRenderTargets(0, nullptr, nullptr);
	if (m_rtv) m_rtv.Reset();

	HRESULT hr = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
	{
		return;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return;
	}

	hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_rtv);
	if (FAILED(hr))
	{
		return;
	}

	// set new viewport
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_context->RSSetViewports(1, &viewport);
}
