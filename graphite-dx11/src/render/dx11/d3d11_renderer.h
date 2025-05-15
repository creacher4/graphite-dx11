#pragma once

#include "render/renderer.h"
#include "render/graph/render_graph.h"

#include <d3d11.h>
#include <wrl/client.h>

class D3D11Renderer : public Renderer
{
public:
	bool Initialize(HWND hwnd, int width, int height) override;
	void RenderFrame(const FrameRenderContext& context) override;
	void Resize(int width, int height) override;

private:
	RenderGraph m_renderGraph;

	//
	// TO-DO
	// add device, context, swapchain etc.

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
};