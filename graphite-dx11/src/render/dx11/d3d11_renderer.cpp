#include "d3d11_renderer.h"
#include "utils/errors.h"
#include <d3dcompiler.h>
#include "render/common/vertex.h"

bool D3D11Renderer::Initialize(HWND hwnd, int width, int height)
{
	m_deviceManager = std::make_unique<D3D11Device>();
	if (!m_deviceManager->Initialize(hwnd, width, height)) return false;

	SetViewport(width, height);

	CreateGBufferTextures(width, height);
	RegisterGBufferWithRenderGraph();

	if (!LoadShaders()) return false;
	if (!CreateTriangleGeometry()) return false;

	SetupRenderGraph();

	return true;
}

void D3D11Renderer::RenderFrame(const FrameRenderContext& context)
{
	m_renderGraph.Execute(context);
	m_deviceManager->GetSwapChain()->Present(1, 0);
}

void D3D11Renderer::Resize(int width, int height)
{
	m_deviceManager->Resize(width, height);
	SetViewport(width, height);

	CreateGBufferTextures(width, height);
	RegisterGBufferWithRenderGraph();
}

void D3D11Renderer::SetViewport(int width, int height)
{
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	auto ctx = m_deviceManager->GetContext();
	ctx->RSSetViewports(1, &viewport);
}

void D3D11Renderer::SetupRenderGraph()
{
	RenderPass clearPass;
	clearPass.name = "ClearBackBuffer";
	clearPass.execute = [this](const FrameRenderContext& context)
		{
			auto ctx = m_deviceManager->GetContext();
			auto rtv = m_deviceManager->GetRenderTargetView();

			ctx->OMSetRenderTargets(1, &rtv, nullptr);
			ctx->ClearRenderTargetView(rtv, context.clearColor);
		};

	clearPass.inputResources = {};
	clearPass.outputResources = {
		{ "Backbuffer", AccessType::Write, ResourceType::Texture2D }
	};

	m_renderGraph.AddPass(clearPass);

	RenderPass geometryPass;
	geometryPass.name = "GeometryPass";

	geometryPass.execute = [this](const FrameRenderContext& context)
		{
			auto ctx = m_deviceManager->GetContext();

			ID3D11RenderTargetView* gbuffers[2] = {
				m_renderGraph.GetExternalResource("Albedo").rtv,
				m_renderGraph.GetExternalResource("Normals").rtv
			};

			auto dsv = m_deviceManager->GetDepthStencilView();

			ctx->OMSetRenderTargets(2, gbuffers, dsv);
			ctx->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			ctx->OMSetDepthStencilState(m_deviceManager->GetDepthStencilState(), 0);

			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			ID3D11Buffer* vb = m_vertexBuffer.Get();

			ctx->IASetInputLayout(m_inputLayout.Get());
			ctx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
			ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			ctx->VSSetShader(m_vs.Get(), nullptr, 0);
			ctx->PSSetShader(m_ps.Get(), nullptr, 0);

			ctx->Draw(3, 0);

		};

	geometryPass.inputResources = {};
	geometryPass.outputResources = {
		{ "Albedo", AccessType::Write, ResourceType::Texture2D },
		{ "Normals", AccessType::Write, ResourceType::Texture2D }
	};

	m_renderGraph.AddPass(geometryPass);

	RenderPass lightingPass;
	lightingPass.name = "LightingPass";

	lightingPass.inputResources = {
		{ "Albedo", AccessType::Read, ResourceType::Texture2D },
		{ "Normals", AccessType::Read, ResourceType::Texture2D }
	};
	lightingPass.outputResources = {
		{ "Backbuffer", AccessType::Write, ResourceType::Texture2D }
	};

	lightingPass.execute = [this](const FrameRenderContext& context)
		{
			auto ctx = m_deviceManager->GetContext();
			auto rtv = m_deviceManager->GetRenderTargetView();

			ctx->OMSetRenderTargets(1, &rtv, nullptr);

			ID3D11ShaderResourceView* srvs[2] = {
				m_renderGraph.GetExternalResource("Albedo").srv,
				m_renderGraph.GetExternalResource("Normals").srv
			};

			ctx->VSSetShader(m_fullscreen_vs.Get(), nullptr, 0);
			ctx->PSSetShader(m_lighting_ps.Get(), nullptr, 0);

			ctx->PSSetShaderResources(0, 2, srvs);
			ctx->PSSetSamplers(0, 1, m_sampler.GetAddressOf());

			ctx->IASetInputLayout(nullptr);
			ctx->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
			ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			ctx->Draw(3, 0);

			// then unbind srvs
			ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
			ctx->PSSetShaderResources(0, 2, nullSRV);
		};

	m_renderGraph.AddPass(lightingPass);

}

void D3D11Renderer::CreateGBufferTextures(int width, int height)
{
	auto create = [&](DXGI_FORMAT format) -> GBufferTarget
	{
		GBufferTarget result;
		auto device = m_deviceManager->GetDevice();

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		device->CreateTexture2D(&desc, nullptr, &result.texture);
		device->CreateRenderTargetView(result.texture.Get(), nullptr, &result.rtv);
		device->CreateShaderResourceView(result.texture.Get(), nullptr, &result.srv);

		return result;
	};

	m_gbuffer["Albedo"] = create(DXGI_FORMAT_R8G8B8A8_UNORM);
	m_gbuffer["Normals"] = create(DXGI_FORMAT_R16G16B16A16_FLOAT);
}

void D3D11Renderer::RegisterGBufferWithRenderGraph()
{
	for (const auto& [name, target] : m_gbuffer)
	{
		m_renderGraph.ImportResource(name, target.rtv.Get(), target.srv.Get());
	}
}

bool D3D11Renderer::LoadShaders()
{
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	auto device = m_deviceManager->GetDevice();


	// geometry pass shaders
	HRESULT hr = D3DReadFileToBlob(L"geometry_vs.cso", &vsBlob);
	if (FAILED(hr)) 
	{
		d3d::LogIfFailed(hr, "Load geometry_vs.cso");
		return false;
	}

	hr = D3DReadFileToBlob(L"geometry_ps.cso", &psBlob);
	if (FAILED(hr))
	{
		d3d::LogIfFailed(hr, "Load geometry_ps.cso");
		return false;
	}

	device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vs);
	device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_ps);

	// input layout for geometry pass
	D3D11_INPUT_ELEMENT_DESC layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                        D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
		layout,
		ARRAYSIZE(layout),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&m_inputLayout
	);
	if (FAILED(hr))
	{
		d3d::LogIfFailed(hr, "CreateInputLayout");
		return false;
	}

	// lighting pass shaders
	Microsoft::WRL::ComPtr<ID3DBlob> fullscreenVSBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> lightingPSBlob;

	hr = D3DReadFileToBlob(L"fullscreen_quad_vs.cso", &fullscreenVSBlob);
	if (FAILED(hr))
	{
		d3d::LogIfFailed(hr, "Load fullscreen_quad_vs.cso");
		return false;
	}

	hr = D3DReadFileToBlob(L"lighting_ps.cso", &lightingPSBlob);
	if (FAILED(hr))
	{
		d3d::LogIfFailed(hr, "Load lighting_ps.cso");
		return false;
	}

	device->CreateVertexShader(fullscreenVSBlob->GetBufferPointer(), fullscreenVSBlob->GetBufferSize(), nullptr, &m_fullscreen_vs);
	device->CreatePixelShader(lightingPSBlob->GetBufferPointer(), lightingPSBlob->GetBufferSize(), nullptr, &m_lighting_ps);

	// create sampler for lighting pass
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&sampDesc, &m_sampler);
	if (FAILED(hr))
	{
		d3d::LogIfFailed(hr, "CreateSamplerState");
		return false;
	}

	return true;
}

bool D3D11Renderer::CreateTriangleGeometry()
{
	Vertex vertices[] = {
		{ {  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
	};

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(vertices);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;

	auto device = m_deviceManager->GetDevice();
	return SUCCEEDED(device->CreateBuffer(&desc, &initData, &m_vertexBuffer));
}


