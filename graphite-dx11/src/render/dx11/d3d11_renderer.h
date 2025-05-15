#pragma once

#include "render/renderer.h"
#include "render/graph/render_graph.h"

#include <d3d11.h>
#include <wrl/client.h>

struct GBufferTarget
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};

class D3D11Renderer : public Renderer
{
public:
	bool Initialize(HWND hwnd, int width, int height) override;
	void RenderFrame(const FrameRenderContext& context) override;
	void Resize(int width, int height) override;

private:
	RenderGraph m_renderGraph;
	std::unordered_map<std::string, GBufferTarget> m_gbuffer;

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;

	// shader stuff
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

	bool LoadShaders();
	bool CreateTriangleGeometry();

	// helpers
	bool CreateDeviceAndSwapchain(HWND hwnd, int width, int height);
	bool CreateRenderTargetView();
	void SetViewport(int width, int height);
	void SetupRenderGraph();

	void CreateGBufferTextures(int width, int height);
	void RegisterGBufferWithRenderGraph();
};