#include "d3d11_renderer.h"
#include "utils/errors.h"
#include <d3dcompiler.h>
#include "render/common/vertex.h"

bool D3D11Renderer::Initialize(HWND hwnd, int width, int height)
{
	if (!CreateDeviceAndSwapchain(hwnd, width, height)) return false;
	if (!CreateRenderTargetView()) return false;

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
	m_swapChain->Present(1, 0);
}

void D3D11Renderer::Resize(int width, int height)
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

	if (!CreateRenderTargetView()) return;
	SetViewport(width, height);
}

bool D3D11Renderer::CreateDeviceAndSwapchain(HWND hwnd, int width, int height)
{
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
		d3d::LogIfFailed(hr, "D3D11CreateDeviceAndSwapChain");
		return false;
	}

	return true;
}

bool D3D11Renderer::CreateRenderTargetView()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
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

void D3D11Renderer::SetViewport(int width, int height)
{
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_context->RSSetViewports(1, &viewport);
}

void D3D11Renderer::SetupRenderGraph()
{
	RenderPass clearPass;
	clearPass.name = "ClearBackBuffer";
	clearPass.execute = [this](const FrameRenderContext& context)
		{
			m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), nullptr);
			m_context->ClearRenderTargetView(m_rtv.Get(), context.clearColor);
		};

	clearPass.outputResources = {
		{ "Backbuffer", AccessType::Write, ResourceType::Texture2D }
	};

	m_renderGraph.AddPass(clearPass);

	RenderPass geometryPass;
	geometryPass.name = "GeometryPass";

	geometryPass.execute = [this](const FrameRenderContext& context)
		{
			ID3D11RenderTargetView* gbuffers[2] = {
				m_renderGraph.GetExternalResource("Albedo").rtv,
				m_renderGraph.GetExternalResource("Normals").rtv
			};

			m_context->OMSetRenderTargets(2, gbuffers, nullptr);

			UINT stride = sizeof(float) * 6;
			UINT offset = 0;
			ID3D11Buffer* vb = m_vertexBuffer.Get();

			m_context->IASetInputLayout(m_inputLayout.Get());
			m_context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
			m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_context->VSSetShader(m_vs.Get(), nullptr, 0);
			m_context->PSSetShader(m_ps.Get(), nullptr, 0);

			m_context->Draw(3, 0);

		};

	geometryPass.outputResources = {
		{ "Albedo", AccessType::Write, ResourceType::Texture2D },
		{ "Normals", AccessType::Write, ResourceType::Texture2D }
	};

	m_renderGraph.AddPass(geometryPass);
}

void D3D11Renderer::CreateGBufferTextures(int width, int height)
{
	auto create = [&](DXGI_FORMAT format) -> GBufferTarget
	{
		GBufferTarget result;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_device->CreateTexture2D(&desc, nullptr, &result.texture);
		m_device->CreateRenderTargetView(result.texture.Get(), nullptr, &result.rtv);
		m_device->CreateShaderResourceView(result.texture.Get(), nullptr, &result.srv);

		return result;
	};

	m_gbuffer["Albedo"] = create(DXGI_FORMAT_R8G8B8A8_UNORM);
	m_gbuffer["Normals"] = create(DXGI_FORMAT_R16G16B16A16_FLOAT);
}

void D3D11Renderer::RegisterGBufferWithRenderGraph()
{
	for (const auto& [name, target] : m_gbuffer)
	{
		m_renderGraph.RegisterExternalResource(name, target.rtv.Get(), target.srv.Get());
	}
}

bool D3D11Renderer::LoadShaders()
{
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;

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

	m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vs);
	m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_ps);

	// input layout
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                              D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(Vertex),             D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_device->CreateInputLayout(
		layout,
		ARRAYSIZE(layout),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&m_inputLayout
	);

	return true;
}

bool D3D11Renderer::CreateTriangleGeometry()
{
	Vertex vertices[] = {
	{ { 0.0f,  0.5f, 0.0f }, { 0, 0, -1 } },
	{ { 0.5f, -0.5f, 0.0f }, { 0, 0, -1 } },
	{ { -0.5f, -0.5f, 0.0f }, { 0, 0, -1 } }
	};

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(vertices);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;

	return SUCCEEDED(m_device->CreateBuffer(&desc, &initData, &m_vertexBuffer));
}


