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
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_dsv.Get(); }
	ID3D11DepthStencilState* GetDepthStencilState() const { return m_depthStencilState.Get(); }

private:
	bool CreateDeviceAndSwapchain(HWND hwnd, int width, int height);
	bool CreateBackBufferRTV();
	bool CreateDepthBuffer(int width, int height);
	bool CreateDepthStencilState();

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
};