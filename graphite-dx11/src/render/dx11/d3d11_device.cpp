#include "d3d11_device.h"
#include "utils/errors.h"

bool D3D11Device::Initialize(HWND hwnd, int width, int height)
{
	if (!CreateDeviceAndSwapchain(hwnd, width, height)) return false;
	if (!CreateBackBufferRTV()) return false;
	return true;
}

bool D3D11Device::CreateDeviceAndSwapchain(HWND hwnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 1;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = hwnd;
    desc.SampleDesc.Count = 1;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL level;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &desc,
        &m_swapChain,
        &m_device,
        &level,
        &m_context
    );

    if (FAILED(hr))
    {
        d3d::LogIfFailed(hr, "CreateDeviceAndSwapchain");
        return false;
    }

    return true;
}

bool D3D11Device::CreateBackBufferRTV()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr))
	{
		d3d::LogIfFailed(hr, "GetBuffer");
		return false;
	}

	hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_rtv);
	if (FAILED(hr))
	{
		d3d::LogIfFailed(hr, "CreateRenderTargetView");
		return false;
	}

	return true;
}

void D3D11Device::Resize(int width, int height)
{
	if (m_context)
    {
		m_context->OMSetRenderTargets(0, nullptr, nullptr);
	}

    m_rtv.Reset();

	HRESULT hr = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
	{
		d3d::LogIfFailed(hr, "ResizeBuffers");
		return;
	}

    CreateBackBufferRTV();
}
