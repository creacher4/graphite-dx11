#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <Windows.h>

class D3D11Device
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void Resize(int width, int height);

	ID3D11Device* GetDevice() const { return m_device.Get(); }
	ID3D11DeviceContext* GetContext() const { return m_context.Get(); }
	IDXGISwapChain* GetSwapChain() const { return m_swapChain.Get(); }
	ID3D11RenderTargetView* GetRenderTargetView() const { return m_rtv.Get(); }

private:
	bool CreateDeviceAndSwapchain(HWND hwnd, int width, int height);
	bool CreateBackBufferRTV();

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
};