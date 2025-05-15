#pragma once

#include "render/renderer.h"
#include "render/graph/render_graph.h"

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
};