#pragma once

#include "render/renderer.h"
#include "render/graph/render_graph.h"
#include "d3d11_device.h"

#include <d3d11.h>
#include <memory>
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

	std::unique_ptr<D3D11Device> m_deviceManager;

	// shader stuff
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

	bool LoadShaders();
	bool CreateTriangleGeometry();

	// helpers
	void SetViewport(int width, int height);
	void SetupRenderGraph();

	void CreateGBufferTextures(int width, int height);
	void RegisterGBufferWithRenderGraph();
};