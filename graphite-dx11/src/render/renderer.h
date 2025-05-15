#pragma once

#include <Windows.h>
#include "render/context/frame_render_context.h"

class Renderer
{
public:
	virtual ~Renderer() = default;

	virtual bool Initialize(HWND hwnd, int width, int height) = 0;
	virtual void RenderFrame(const FrameRenderContext& context) = 0;
	virtual void Resize(int width, int height) = 0;
};
